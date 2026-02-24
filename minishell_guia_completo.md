# MINISHELL — Guia Técnico Completo
**fshiniti / abroslav — 42 Porto — Fevereiro 2026**

---

## 1. O que é o Minishell?

Uma shell bash-like escrita em C como projeto da 42. Lê input do utilizador, faz parse e executa comandos com suporte a pipes, redirections, heredocs, variáveis de ambiente e builtins.

Compilação: `cc -Wall -Wextra -Werror -g -fsanitize=address -lreadline`

---

## 2. Estrutura do Projeto

```
src/
  main.c                    — entry point, shell loop
  utils/
    main_utils.c            — validate_line(), process_line()
    free.c                  — free_all(), free_tokens(), free_cmds()
    utils.c                 — special_expand_params(), ft_strcmp(), free_tab()
    utils2.c                — utilitários auxiliares
    debug.c                 — print_tokens(), print_cmds() (debug only)
  lexer/
    lexer.c                 — lexer() tokenizador principal
    lexer_utils.c           — is_space(), is_special(), join_and_free()
    lexer_word.c            — build_full_word(), handle_word_part()
  expander/
    expander.c              — expand_vars(), expand_tokens()
  parser/
    parser.c                — parser(), validate_syntax()
    parser_utils.c          — new_cmd(), cmd_add_arg(), redir_add_back()
    parser_redir.c          — parse_redir_in(), parse_redir_out()
  heredoc/
    heredoc.c               — handle_heredoc(), process_heredocs()
    heredoc_utils.c         — heredoc_has_quotes(), heredoc_remove_quotes(), heredoc_gen_temp_filename()
  exec/
    execute.c               — executor(), execute_pipe(), wait_children()
    child.c                 — child_process(), handle_pipes()
    redirs.c                — handle_redirection(), apply_redir()
    path.c                  — find_path()
    exec_errors.c           — execution_error(), is_right_assignment()
  builtins/
    builtins_router.c       — is_builtin(), exec_builtin()
    builtins_info.c         — ft_echo(), ft_pwd()
    builtin_cd.c            — ft_cd()
    builtin_exit.c          — ft_exit()
    builtins_env.c          — ft_export(), ft_unset(), ft_env()
    export_print.c          — show_export_list()
  signals/
    signals.c               — setup_signals(), setup_signals_execution(), setup_signals_heredoc()
includes/
  minishell.h               — todas as structs, enums e protótipos
libft/                      — biblioteca C customizada
```

---

## 3. Estruturas de Dados

### 3.1 t_shell — Estado global da shell

```c
typedef struct s_shell
{
    char    **env_vars;   // array de variáveis de ambiente (KEY=VALUE, terminado em NULL)
    int     exit_code;    // código de saída do último comando (usado por $?)
    t_token *s_tokens;    // lista de tokens atual (para cleanup de emergência)
    t_cmd   *s_cmds;      // lista de comandos atual (para cleanup de emergência)
}   t_shell;
```

- `env_vars` é copiado do `envp` do main na inicialização
- `exit_code` é atualizado após cada execução
- `s_tokens` e `s_cmds` existem apenas para cleanup em caso de sinal/crash

### 3.2 t_token — Um token do lexer

```c
typedef enum e_token_type
{
    TK_WORD,        // palavra ou argumento
    TK_PIPE,        // |
    TK_REDIR_IN,    // <
    TK_REDIR_OUT,   // >
    TK_APPEND,      // >>
    TK_HEREDOC,     // <<
}   t_token_type;

typedef struct s_token
{
    t_token_type    type;       // tipo do token
    char            *value;     // valor string do token
    int             no_expand;  // 1 = veio de aspas simples, NÃO expandir
    int             in_dquotes; // 1 = veio de aspas duplas
    struct s_token  *next;      // próximo token na lista ligada
}   t_token;
```

- `no_expand=1` → token veio de aspas simples → `$VAR` NÃO é expandido
- `in_dquotes=1` → garante que strings vazias `""` são adicionadas como argumentos

### 3.3 t_cmd — Um comando na pipeline

```c
typedef struct s_cmd
{
    char        **args;          // array de argumentos terminado em NULL (args[0] = nome do cmd)
    t_redir     *redirs;         // lista ligada de redirections
    char        **limits;        // delimitadores heredoc (campo legacy)
    int         heredoc_fd;      // fd do ficheiro temporário do heredoc (-1 se nenhum)
    struct s_cmd *next;          // próximo comando na pipeline
}   t_cmd;
```

- `args` cresce dinamicamente via `cmd_add_arg()` (padrão realloc)
- `heredoc_fd` começa a `-1`, preenchido por `process_heredocs()` antes de qualquer fork

### 3.4 t_redir — Um redirecionamento

```c
typedef enum e_redir_type
{
    REDIR_IN,       // < ficheiro
    REDIR_OUT,      // > ficheiro
    REDIR_APPEND,   // >> ficheiro
    REDIR_HEREDOC,  // << DELIM
}   t_redir_type;

typedef struct s_redir
{
    t_redir_type    type;    // tipo de redirecionamento
    char            *target; // nome do ficheiro ou delimitador heredoc
    struct s_redir  *next;   // próximo redirecionamento
}   t_redir;
```

### 3.5 t_hd_ctx — Contexto de execução do heredoc

```c
typedef struct s_hd_ctx
{
    int     fd;         // fd do ficheiro temporário (lado escrita)
    int     expand;     // 1 = expandir $VAR dentro do heredoc, 0 = literal
    char    **env;      // ambiente para expansão de variáveis
    int     exit_code;  // último exit code (para $?)
}   t_hd_ctx;
```

---

## 4. Fluxo de Execução Completo

### 4.1 Visão Geral

```
Utilizador escreve: ls -la | grep .c > out.txt
                         |
               [1] get_input()
                         |
               [2] validate_line()        — verifica comentários e aspas por fechar
                         |
               [3] lexer()                — tokeniza em lista t_token
                         |
               [4] expand_tokens()        — substitui $VAR pelos valores
                         |
               [5] parser()               — valida sintaxe, constrói lista t_cmd
                         |
               [6] process_heredocs()     — lê input heredoc ANTES de qualquer fork
                         |
               [7] executor()
                  /              \
       builtin único            pipeline (1+ cmds ou externo)
       exec_single_builtin()    execute_pipe()
               |                        |
       exec_builtin()         para cada cmd: fork()
                                         |
                              child_process()
                                /          \
                         handle_pipes()    handle_redirection()
                                               |
                                          child_exec()
                                          /         \
                                    builtin      cmd externo
                                                 find_path()
                                                 execve()
```

### 4.2 Passo 1 — Input: get_input() e shell_loop()

`main()` chama `shell_loop()` que corre indefinidamente:

1. `get_input()`: se stdin é terminal → `readline("minishell> ")`; senão → `read_line()` (modo não-interativo)
2. Se `readline` devolve `NULL` → EOF → imprime "exit" e sai
3. `add_history(line)` — adiciona ao histórico do readline (setas funcionam)
4. `validate_line()` — salta comentários e deteta aspas por fechar
5. `process_line()` — pipeline completo

```c
static void shell_loop(t_shell *shell)
{
    char *line;
    while (1)
    {
        line = get_input(shell);        // readline ou read_line
        if (!line) { printf("exit\n"); break; }
        if (*line) add_history(line);
        if (validate_line(line, shell)) continue;  // skip se inválido
        process_line(line, shell);
        free(line);
    }
}
```

### 4.3 Passo 2 — Validação: validate_line()

Dois testes em sequência:

1. **`is_comment_line()`**: percorre espaços, verifica se o 1.º char visível é `#` → `exit_code=0`, salta
2. **`has_unclosed_quotes()`**: percorre a string; quando encontra `'` ou `"` procura o fecho; se chega ao `\0` sem fechar → erro de sintaxe, `exit_code=2`, salta

### 4.4 Passo 3 — Lexer: tokenizar o input

`lexer()` percorre o input char a char via `process_char()`:

| Caracter | Resultado |
|----------|-----------|
| espaço/tab | ignorado |
| `\|` | token `TK_PIPE` |
| `<` seguido de `<` | token `TK_HEREDOC` |
| `<` | token `TK_REDIR_IN` |
| `>` seguido de `>` | token `TK_APPEND` |
| `>` | token `TK_REDIR_OUT` |
| outro | `build_full_word()` |

**`build_full_word()`** (lexer_word.c) constrói uma palavra completa incluindo secções com aspas:

- **Aspas simples** `'abc'` → extrai conteúdo literal SEM expansão, seta `no_expand=1`
- **Aspas duplas** `"abc$X"` → extrai conteúdo E expande `$VAR` dentro, seta `in_dquotes=1`
- **Sem aspas** → extrai até whitespace/char especial, expande `$VAR`

Resultado para `ls -la | grep .c > out.txt`:
```
[TK_WORD:"ls"] → [TK_WORD:"-la"] → [TK_PIPE:"|"] → [TK_WORD:"grep"] → [TK_WORD:".c"] → [TK_REDIR_OUT:">"] → [TK_WORD:"out.txt"] → NULL
```

### 4.5 Passo 4 — Expander: expand_vars() e expand_tokens()

`expand_tokens()` percorre a lista de tokens. Para cada `TK_WORD` com `no_expand=0`, chama `expand_vars()`.

`expand_vars()` percorre a string caracter a caracter:

| Padrão | Resultado |
|--------|-----------|
| `$?` | `ft_itoa(last_exit)` — código de saída do último comando |
| `$$` | `ft_itoa(getpid())` — PID da shell |
| `$1`.`$9` | string vazia (positional params não suportados) |
| `$NOME` | `get_env_value(env, "NOME")` ou string vazia se não existir |
| `$` no fim | `"$"` literal |
| qualquer outro char | acrescentado literalmente |

O resultado é uma nova string malloc'd que substitui `token->value`.

### 4.6 Passo 5 — Parser: validate_syntax() e construção da lista t_cmd

**`validate_syntax()`** verifica:
- 1.º token é `TK_PIPE` → erro de sintaxe
- `TK_PIPE` seguido de outro `TK_PIPE` ou fim → erro de sintaxe
- Qualquer token de redirect não seguido de `TK_WORD` → erro de sintaxe
- Em caso de erro: `exit_code=2`, devolve `NULL`

**`parser()`** constrói a lista `t_cmd`:
- Começa com um `t_cmd` vazio
- `TK_WORD` → `cmd_add_arg(current_cmd, token->value)`
  - Caso especial: valor vazio E `no_expand=0` E `in_dquotes=0` → salta (resultado de expansão de var não definida)
- `TK_PIPE` → cria novo `t_cmd`, adiciona à lista, torna-o o atual
- `TK_REDIR_IN` / `TK_HEREDOC` → `parse_redir_in()`
- `TK_REDIR_OUT` / `TK_APPEND` → `parse_redir_out()`

**`parse_redir_in()` / `parse_redir_out()`**:
- Aloca `t_redir`, define o tipo
- Avança o ponteiro de token para obter ficheiro/delimitador
- Para heredoc com `no_expand=1` (delimitador entre aspas como `'EOF'`): envolve o target em aspas simples → `heredoc_has_quotes()` deteta e desativa expansão
- Adiciona redir à lista `current_cmd->redirs`

Resultado para `ls -la | grep .c > out.txt`:
```
cmd1: args=["ls", "-la", NULL]   redirs=NULL
cmd2: args=["grep", ".c", NULL]  redirs=[REDIR_OUT:"out.txt"]
```

### 4.7 Passo 6 — Heredoc: process_heredocs()

Chamado pelo `executor()` **ANTES** de qualquer fork. Percorre toda a lista `t_cmd`.

Para cada `REDIR_HEREDOC` encontrado:

1. `handle_heredoc(delimiter, env, last_exit)` é chamado
2. `heredoc_has_quotes(delimiter)` → verifica se delimitador contém `'` ou `"`
3. `heredoc_remove_quotes(delimiter)` → remove as aspas para obter delimitador limpo
4. `ctx.expand = !heredoc_has_quotes()` → desativa expansão se delimitador tinha aspas
5. `heredoc_gen_temp_filename()` → gera `.heredoc_tmp_0`, `.heredoc_tmp_1`, etc.
6. Abre ficheiro temporário para escrita (`O_CREAT|O_WRONLY|O_TRUNC`)
7. `setup_signals_heredoc()` → Ctrl+C fecha STDIN em vez de redesenhar prompt
8. Loop de leitura:
   - `readline("> ")` (interativo) ou `heredoc_read_line()` (não-interativo)
   - Linha == delimitador → para
   - Ctrl+C (`g_last_signal == SIGINT`) → cleanup, devolve `-1`
   - EOF sem delimitador → imprime warning, para
   - Cada linha: se `expand` → `expand_vars()`, depois escreve no ficheiro temporário
9. Fecha fd de escrita, reabre ficheiro temporário `O_RDONLY`
10. `unlink(temp)` → ficheiro apagado do filesystem mas fd continua válido
11. Guarda fd em `cmd->heredoc_fd`

### 4.8 Passo 7 — Executor: executor()

```
1. process_heredocs()             — sempre primeiro
2. Comando único E args[0] é VAR=VALUE?
   → update_env(), exit_code=0, return
3. Comando único E is_builtin()?
   → exec_single_builtin()        — corre no processo pai
4. Caso contrário:
   → execute_pipe()               — faz fork de processos filhos
```

**`exec_single_builtin()`** (builtins no processo pai):
- `dup()` stdin e stdout para os guardar
- `handle_redirection()` — aplica redirections
- `exec_builtin()` — corre o builtin
- `dup2()` restaura stdin/stdout originais
- **Crítico**: builtins como `cd`, `export`, `unset` TÊM de correr no pai para afetar o estado da shell

**`execute_pipe()`**:
- `setup_signals_execution()`: pai ignora SIGINT e SIGQUIT durante execução
- `pipe_loop()`: para cada comando na pipeline:
  1. Se há próximo comando: `pipe(fd_pipe)` cria novo pipe
  2. `fork()` — cria processo filho
  3. Pai: fecha write end do pipe, guarda read end como próximo `fd_in`
  4. Filho: `child_process()`
- `wait_children()`: `waitpid()` no último PID, depois espera por todos os outros
  - `WIFEXITED` → `exit_code = WEXITSTATUS(status)`
  - `WIFSIGNALED` → se `SIGQUIT` → `"Quit (core dump)\n"`, `exit_code = 128 + sinal`
- `setup_signals()` restaura handling normal de sinais

**`child_process()`**:
1. Reset SIGINT e SIGQUIT para `SIG_DFL` — filhos tratam sinais normalmente
2. `handle_pipes()`: conecta file descriptors do pipe
   - `fd_in != -1`: `dup2(fd_in, STDIN_FILENO)`, fecha `fd_in`
   - `cmd->next != NULL`: fecha `fd_pipe[0]`, `dup2(fd_pipe[1], STDOUT_FILENO)`, fecha `fd_pipe[1]`
3. `handle_redirection()`: aplica `<`, `>`, `>>`, `<<` para este comando
   - `REDIR_IN`: `open(file, O_RDONLY)`, dup2 para STDIN
   - `REDIR_OUT`: `open(file, O_WRONLY|O_CREAT|O_TRUNC)`, dup2 para STDOUT
   - `REDIR_APPEND`: `open(file, O_WRONLY|O_CREAT|O_APPEND)`, dup2 para STDOUT
   - `REDIR_HEREDOC`: `dup2(cmd->heredoc_fd, STDIN_FILENO)`
   - Devolve `1` em caso de erro → `cleanup_exit_child(shell, 1)`
4. `child_exec()`:
   - Sem args → exit 0
   - `args[0]` é string vazia → `execution_error(127)`
   - `is_builtin()` → `exec_builtin()`, `cleanup_exit_child()`
   - Caso contrário: `resolve_cmd_path()`
     - Tem `/`: verifica `access()`, `execution_error(127)` se não encontrado
     - Sem `/`: `find_path()` — divide PATH por `:`, tenta cada dir + "/" + cmd
   - `execve(path, args, env_vars)`
   - Se `execve` falha: `errno==EACCES` → exit 126, senão exit 1

### 4.9 Passo 8 — Limpeza de Memória

Após `executor()` retornar:
```c
free_cmds(cmds);      // liberta arrays args, listas redir (strings target), fecha heredoc_fd
free_tokens(tokens);  // liberta valores dos tokens e nós
shell->s_tokens = NULL;
shell->s_cmds = NULL;
```

No exit (`ft_exit` ou EOF):
```c
free_all(shell);  // chama free_tokens, free_cmds, free_env, rl_clear_history
```

---

## 5. Gestão de Sinais (3 modos)

| Modo | SIGINT (Ctrl+C) | SIGQUIT (Ctrl+\\) |
|------|-----------------|-------------------|
| Interativo (prompt) | Imprime newline, redesenha prompt vazio. `g_last_signal=SIGINT`, `exit_code=130` | Ignorado |
| Durante execução | Pai ignora. Filho recebe SIG_DFL (termina) | Pai ignora. Filho recebe SIG_DFL (core dump + msg) |
| Durante heredoc | Fecha STDIN, `g_last_signal=SIGINT` → readline devolve NULL → aborta heredoc | Ignorado |

**`setup_signals()`** — modo interativo:
- SIGINT → `handle_sigint()`: `write("\n")`, `rl_on_new_line()`, `rl_replace_line("",0)`, `rl_redisplay()`
- SIGQUIT → `SIG_IGN`

**`setup_signals_execution()`** — durante `execute_pipe()`:
- SIGINT e SIGQUIT → `SIG_IGN` no pai
- Filhos fazem reset para `SIG_DFL` no início de `child_process()`

**`setup_signals_heredoc()`** — durante leitura do heredoc:
- SIGINT → `handle_sigint_heredoc()`: `g_last_signal=SIGINT`, `close(STDIN_FILENO)`
- SIGQUIT → `SIG_IGN`

---

## 6. Referência dos Builtins

Todos os builtins correm no processo pai (exceto dentro de uma pipeline, onde correm num filho).

| Builtin | Comportamento |
|---------|---------------|
| `echo [-n] [args]` | Imprime args separados por espaços. `-n` suprime newline. Múltiplas flags `-n` consecutivas aceites |
| `pwd` | Imprime valor da var `PWD`; fallback para `getcwd()` |
| `cd [dir]` | Sem arg / `~`: vai para `$HOME`. `-`: vai para `$OLDPWD` e imprime-o. Atualiza `PWD` e `OLDPWD` via `ft_export` |
| `export [NOME=VAL]` | Sem args: imprime todas as vars ordenadas (formato `declare -x`). Com args: valida nome, adiciona/atualiza em `env_vars` |
| `unset NOME...` | Remove variáveis de `env_vars`. Sem erro se não existir |
| `env` | Imprime todas as variáveis de ambiente (formato `KEY=VALUE`) |
| `exit [n]` | Imprime "exit". Sem arg: sai com `last_exit_code`. Arg numérico: sai com `(unsigned char)n`. Não numérico: "numeric argument required", exit 2. Demasiados args: erro, fica na shell |

---

## 7. Gestão de Variáveis de Ambiente

`env_vars` é um array `char**` terminado em NULL guardado em `t_shell`.
Cada elemento é uma string malloc'd `"KEY=VALUE"` ou `"KEY"` (para vars sem valor).

**`get_env_value(env, key)`**: scan linear, devolve ponteiro para a parte VALUE (após `=`), ou NULL.

**`update_env(arg, env)`**: se KEY já existe → substitui string. Se não → `add_var_to_env()` que realoca o array com +1.

**`ft_export` sem args** → `show_export_list()`: recolhe todas as vars, ordena alfabeticamente, imprime como `declare -x KEY="VALUE"`.

**`shlvl_update()`**: lê `SHLVL`, incrementa por 1. Se > 999 → warning, reset para 1. Se < 0 → define como 0.

---

## 8. Códigos de Erro

| Código | Significado |
|--------|-------------|
| 0 | Sucesso |
| 1 | Erro geral |
| 2 | Erro de sintaxe (pipe inválido, aspas por fechar, redirect inválido) |
| 126 | Permissão negada / É um diretório |
| 127 | Comando não encontrado / Ficheiro não existe |
| 128+N | Morto pelo sinal N |
| 130 | Morto por Ctrl+C (SIGINT = sinal 2, 128+2) |

---

## 9. Trace Completo de Execução

**Comando:** `echo "hello $USER" | cat -n > result.txt`

**Passo 1 — Input:** `readline` devolve: `echo "hello $USER" | cat -n > result.txt`

**Passo 2 — validate_line:** sem comentário, sem aspas por fechar → processa

**Passo 3 — Tokens do Lexer:**
```
[TK_WORD:"echo"] [TK_WORD:"hello $USER"(in_dquotes=1)] [TK_PIPE] [TK_WORD:"cat"] [TK_WORD:"-n"] [TK_REDIR_OUT] [TK_WORD:"result.txt"]
```

**Passo 4 — Expander:** `"hello $USER"` → `"hello fshiniti"` (expande dentro de aspas duplas)

**Passo 5 — Parser:**
```
cmd1: args=["echo","hello fshiniti"]  redirs=NULL
cmd2: args=["cat","-n"]               redirs=[REDIR_OUT:"result.txt"]
```

**Passo 6 — process_heredocs:** sem heredoc → nada a fazer

**Passo 7 — executor:** 2 comandos → `execute_pipe()`
```
pipe(fd_pipe) → fd_pipe[0]=read end, fd_pipe[1]=write end

fork() filho1 para echo:
  handle_pipes: cmd->next existe → dup2(fd_pipe[1], STDOUT) → echo escreve no pipe
  sem redirections
  is_builtin("echo") → exec_builtin → ft_echo → escreve "hello fshiniti\n" no pipe
  cleanup_exit_child(0)

pai: close(fd_pipe[1]), fd_in=fd_pipe[0]

fork() filho2 para cat:
  handle_pipes: fd_in != -1 → dup2(fd_in, STDIN) → cat lê do pipe
  handle_redirection: REDIR_OUT "result.txt" → open, dup2 para STDOUT
  find_path("cat") → "/bin/cat"
  execve("/bin/cat", ["cat","-n"], env)

pai: close(fd_in)
wait_children: waitpid(pid_filho2) → exit_code = código de saída do cat
```

**Passo 8 — Cleanup:** `free_cmds`, `free_tokens`

**Resultado:** ficheiro `result.txt` contém `     1	hello fshiniti`

---

## 10. Conformidade Norminette

Regras aplicadas:
- Máximo 25 linhas por função
- Máximo 5 variáveis locais por função
- Máximo 4 parâmetros por função
- Variáveis declaradas no início da função
- Sem `for` loops (usa `while`)
- Sem comentários inline (apenas comentários em bloco no início das funções)
- Uma instrução por linha
- Variável global: apenas `g_last_signal` (int) — permitida pelo subject

**Decisões de design motivadas pela Norminette:**
- `t_hd_ctx` struct: agrupa 4 parâmetros do heredoc em 1 para passar a `read_heredoc_lines()`
- heredoc dividido em `heredoc.c` (4 funções) e `heredoc_utils.c` (4 funções)
- `parser_redir.c`: `new_redir()` extraído para evitar repetição e manter funções curtas
- exec dividido em `execute.c`, `child.c`, `redirs.c`, `path.c`, `exec_errors.c`

---

## 11. Refactor de parser_redir.c (Fevereiro 2026)

O ficheiro foi refatorado para:

1. **Eliminar duplicação** — `new_redir(type)` centraliza a alocação e inicialização
2. **Corrigir quote wrapping** — delimitadores heredoc de aspas simples agora ficam com `'` em vez de `"` via `quoted_target()`
3. **Clareza** — tipo é determinado primeiro, depois alocação, depois target

```c
// Antes (repetição em parse_redir_in e parse_redir_out):
redir = malloc(sizeof(t_redir));
if (!redir) return;
redir->next = NULL;
redir->target = NULL;

// Depois (função auxiliar):
static t_redir *new_redir(t_redir_type type)
{
    t_redir *redir = malloc(sizeof(t_redir));
    if (!redir) return (NULL);
    redir->type = type;
    redir->target = NULL;
    redir->next = NULL;
    return (redir);
}
```
