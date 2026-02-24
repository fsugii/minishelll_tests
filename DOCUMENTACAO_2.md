# 📖 Documentação Completa — Minishell

> **Guia detalhado de funcionamento para leigos**
> Autores: fshiniti & abroslav — 42 Porto

---

## 📑 Índice

1. [Visão Geral](#1--visão-geral)
2. [Estrutura do Projeto](#2--estrutura-do-projeto)
3. [Estruturas de Dados (Structs)](#3--estruturas-de-dados-structs)
4. [Fluxo Completo de Execução](#4--fluxo-completo-de-execução)
   - 4.1 [Inicialização — `main()`](#41-inicialização--main)
   - 4.2 [Loop Principal — `shell_loop()`](#42-loop-principal--shell_loop)
   - 4.3 [Leitura de Input — `get_input()`](#43-leitura-de-input--get_input)
   - 4.4 [Validação — `validate_line()`](#44-validação--validate_line)
   - 4.5 [Processamento da Linha — `process_line()`](#45-processamento-da-linha--process_line)
   - 4.6 [Lexer (Tokenização)](#46-lexer-tokenização)
   - 4.7 [Parser (Construção de Comandos)](#47-parser-construção-de-comandos)
   - 4.8 [Expander (Expansão de Variáveis)](#48-expander-expansão-de-variáveis)
   - 4.9 [Heredoc](#49-heredoc)
   - 4.10 [Executor](#410-executor)
   - 4.11 [Builtins](#411-builtins)
   - 4.12 [Sinais](#412-sinais)
   - 4.13 [Limpeza de Memória](#413-limpeza-de-memória)
5. [Diagrama do Fluxo](#5--diagrama-do-fluxo)
6. [Análise de Norminette](#6--análise-de-norminette)

---

## 1. 🌐 Visão Geral

O **Minishell** é uma reimplementação simplificada do **bash** (Bourne Again Shell). Imagine o Terminal do Linux ou Mac — aquela tela preta onde você digita comandos. Este programa replica esse comportamento:

- Mostra um **prompt** (`minishell> `) e espera você digitar algo.
- **Interpreta** o que você digitou (por exemplo, `ls -la | grep txt > arquivo.txt`).
- **Executa** o comando pedido.
- **Repete** até você digitar `exit` ou pressionar `Ctrl+D`.

O programa lida com:
- **Pipes** (`|`) — encadear a saída de um comando como entrada de outro.
- **Redirecionamentos** (`<`, `>`, `>>`, `<<`) — redirecionar entrada/saída para arquivos.
- **Variáveis de ambiente** (`$HOME`, `$PATH`, `$?`).
- **Aspas** simples (`'`) e duplas (`"`).
- **Builtins** — comandos internos como `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`.
- **Sinais** — `Ctrl+C` (SIGINT) e `Ctrl+\` (SIGQUIT).

---

## 2. 📁 Estrutura do Projeto

```
minishelll_tests/
├── Makefile                    # 🔧 Compilação do projeto
├── includes/
│   └── minishell.h             # 📋 Header principal (structs, protótipos, includes)
├── libft/                      # 📚 Biblioteca auxiliar da 42 (funções ft_*)
│   ├── Makefile
│   ├── libft.h
│   └── (ft_strlen.c, ft_strdup.c, ft_split.c, etc.)
└── src/
    ├── main.c                  # 🚀 Ponto de entrada do programa
    ├── builtins/               # 🏠 Comandos internos do shell
    │   ├── builtins_router.c   #   → Decide qual builtin executar
    │   ├── builtins_info.c     #   → echo e pwd
    │   ├── builtin_cd.c        #   → cd (mudar diretório)
    │   ├── builtin_exit.c      #   → exit (sair do shell)
    │   ├── builtins_env.c      #   → export, unset e env
    │   └── export_print.c      #   → Impressão formatada do export
    ├── env/                    # 🌍 Gestão de variáveis de ambiente
    │   ├── env_init.c          #   → Cópia inicial e SHLVL
    │   ├── env_get.c           #   → Busca de valores
    │   └── env_modify.c        #   → Modificação (add/update)
    ├── exec/                   # ⚡ Motor de execução
    │   ├── execute.c           #   → Executor principal (builtins vs pipes)
    │   ├── child.c             #   → Lógica do processo filho
    │   ├── redirs.c            #   → Aplicação de redirecionamentos
    │   ├── path.c              #   → Busca de executáveis no PATH
    │   └── exec_errors.c       #   → Mensagens de erro de execução
    ├── expander/               # 💱 Expansão de variáveis ($VAR)
    │   └── expander.c
    ├── heredoc/                # 📝 Here-documents (<<)
    │   ├── heredoc.c
    │   └── heredoc_utils.c
    ├── lexer/                  # 🔤 Análise léxica (tokenização)
    │   ├── lexer.c
    │   ├── lexer_utils.c
    │   └── lexer_word.c
    ├── parser/                 # 🧩 Análise sintática
    │   ├── parser.c
    │   ├── parser_utils.c
    │   └── parser_redir.c
    ├── signals/                # 🚦 Tratamento de sinais
    │   └── signals.c
    └── utils/                  # 🛠️ Utilitários gerais
        ├── main_utils.c        #   → Validação e processamento de linhas
        ├── free.c              #   → Liberação de memória
        ├── utils.c             #   → Funções auxiliares
        ├── utils2.c            #   → ft_atoll, read_line
        └── debug.c             #   → Funções de debug (print_tokens, print_cmds)
```

---

## 3. 🧱 Estruturas de Dados (Structs)

### `t_shell` — O "cérebro" do programa

> 📍 Definida em `includes/minishell.h`

```c
typedef struct s_shell
{
    char    **env_vars;     // Array de strings: {"HOME=/home/user", "PATH=/usr/bin", ...}
    int     exit_code;      // Código de saída do último comando (0 = sucesso, 1-255 = erro)
    t_token *s_tokens;      // Ponteiro para a lista de tokens (usado para cleanup)
    t_cmd   *s_cmds;        // Ponteiro para a lista de comandos (usado para cleanup)
} t_shell;
```

**Para um leigo:** Imagine o `t_shell` como a **mesa de controlo** do programa. Nela ficam:
- A **lista de variáveis de ambiente** (`env_vars`) — como uma agenda com todas as configurações do sistema.
- O **código de saída** (`exit_code`) — um número que indica se o último comando deu certo ou errado.
- Referências temporárias para os **tokens** e **comandos** atuais, para que possam ser limpos da memória.

---

### `t_token` — Peças do texto digitado

> 📍 Definida em `includes/minishell.h`

```c
typedef struct s_token
{
    t_token_type    type;       // Tipo: TK_WORD(0), TK_PIPE(1), TK_REDIR_IN(2), etc.
    char            *value;     // O texto em si: "ls", "|", ">", "arquivo.txt"
    int             no_expand;  // 1 se veio de aspas simples (NÃO expandir $VAR)
    int             in_dquotes; // 1 se veio de aspas duplas
    struct s_token  *next;      // Ponteiro para o próximo token (lista encadeada)
} t_token;
```

**Para um leigo:** Quando você digita `echo "Hello" | cat`, o programa quebra isso em pedaços chamados **tokens**:
| Token | type | value |
|-------|------|-------|
| 1 | `TK_WORD` | `echo` |
| 2 | `TK_WORD` | `Hello` |
| 3 | `TK_PIPE` | `\|` |
| 4 | `TK_WORD` | `cat` |

Cada token aponta para o seguinte, formando uma **corrente** (lista encadeada).

---

### `t_cmd` — Comando pronto para executar

> 📍 Definida em `includes/minishell.h`

```c
typedef struct s_cmd
{
    char    **args;         // Argumentos: {"ls", "-la", NULL}
    t_redir *redirs;        // Lista de redirecionamentos
    char    **limits;       // Delimitadores de heredoc
    int     heredoc_fd;     // File descriptor do heredoc (-1 se não há)
    struct s_cmd  *next;    // Próximo comando no pipe
} t_cmd;
```

**Para um leigo:** Se os tokens são as peças do puzzle, o `t_cmd` é o **puzzle montado**. Para `ls -la | grep txt`:
- **Comando 1:** `args = {"ls", "-la", NULL}`, `next` → Comando 2
- **Comando 2:** `args = {"grep", "txt", NULL}`, `next` → `NULL`

---

### `t_redir` — Redirecionamento

> 📍 Definida em `includes/minishell.h`

```c
typedef struct s_redir
{
    t_redir_type    type;    // REDIR_IN(<), REDIR_OUT(>), REDIR_APPEND(>>), REDIR_HEREDOC(<<)
    char            *target; // Nome do ficheiro ou delimitador
    struct s_redir  *next;   // Próximo redirecionamento
} t_redir;
```

---

### `t_hd_ctx` — Contexto do Heredoc

> 📍 Definida em `includes/minishell.h`

```c
typedef struct s_hd_ctx
{
    int     fd;         // File descriptor do ficheiro temporário
    int     expand;     // 1 = expandir variáveis, 0 = não expandir
    char    **env;      // Variáveis de ambiente (para expansão)
    int     exit_code;  // Código de saída (para $?)
} t_hd_ctx;
```

---

## 4. 🔄 Fluxo Completo de Execução

### 4.1 Inicialização — `main()`

> 📍 `src/main.c`

```c
int main(int argc, char **argv, char **envp)
{
    t_shell  shell;

    (void)argc;
    (void)argv;
    init_shell(&shell, envp);      // 1. Inicializa a struct shell
    if (!shell.env_vars)           // 2. Verifica se copiou o ambiente com sucesso
        return (1);
    shlvl_update(&shell.env_vars); // 3. Incrementa SHLVL
    setup_signals();               // 4. Configura sinais
    shell_loop(&shell);            // 5. Entra no loop principal
    free_env(shell.env_vars);      // 6. Limpa variáveis ao sair
    rl_clear_history();            // 7. Limpa histórico do readline
    return (shell.exit_code);      // 8. Retorna o último exit code
}
```

**O que acontece passo a passo:**

1. **`init_shell(&shell, envp)`** — Inicializa a estrutura:
   - `shell->env_vars = copy_env(envp)` → Copia TODAS as variáveis de ambiente do sistema.
   - `shell->exit_code = 0` → Começa com sucesso.
   - `shell->s_tokens = NULL` e `shell->s_cmds = NULL` → Sem tokens/comandos ainda.

2. **`shlvl_update(&shell.env_vars)`** (`src/env/env_init.c`) — Incrementa a variável `SHLVL` (shell level). Se passar de 999, reseta para 1.

3. **`setup_signals()`** (`src/signals/signals.c`) — Configura como o shell reage a `Ctrl+C` e `Ctrl+\`.

4. **`shell_loop(&shell)`** — Entra no loop infinito que lê e executa comandos.

---

### 4.2 Loop Principal — `shell_loop()`

> 📍 `src/main.c`

```c
static void shell_loop(t_shell *shell)
{
    char *line;

    while (1)
    {
        line = get_input(shell);       // 1. Lê o input do utilizador
        if (!line)                     // 2. Se recebeu NULL (Ctrl+D / EOF)
        {
            if (isatty(STDIN_FILENO))
                printf("exit\n");
            break;
        }
        if (*line)
            add_history(line);         // 3. Adiciona ao histórico
        if (validate_line(line, shell))// 4. Valida a linha (aspas, comentários)
            continue;
        process_line(line, shell);     // 5. Processa e executa
        free(line);                    // 6. Liberta a memória da linha
    }
}
```

**Para um leigo:** É como um **garçom num restaurante** — num loop infinito:
1. Espera o cliente (utilizador) fazer o pedido.
2. Se o cliente for embora (`Ctrl+D`), fecha o restaurante.
3. Anota o pedido no bloco (histórico).
4. Verifica se o pedido faz sentido (validação).
5. Passa o pedido para a cozinha (processamento).
6. Limpa a mesa (free).

---

### 4.3 Leitura de Input — `get_input()`

> 📍 `src/main.c`

```c
static char *get_input(t_shell *shell)
{
    char *line;

    if (isatty(STDIN_FILENO))
        line = readline("minishell> ");
    else
        line = read_line();
    if (g_last_signal == SIGINT)
    {
        shell->exit_code = 130;
        g_last_signal = 0;
    }
    return (line);
}
```

**Detalhes importantes:**
- `isatty(STDIN_FILENO)` verifica se a entrada padrão é um terminal interativo ou um ficheiro/pipe.
- `readline("minishell> ")` mostra o prompt e permite edição com setas, `Ctrl+A`, etc.
- `read_line()` em `src/utils/utils2.c` lê caractere a caractere para quando o input não é um terminal.
- `g_last_signal` é a **única variável global** do projeto.

---

### 4.4 Validação — `validate_line()`

> 📍 `src/utils/main_utils.c`

```c
int validate_line(char *line, t_shell *shell)
{
    if (is_comment_line(line))         // Se começa com # (comentário)
    {
        shell->exit_code = 0;
        free(line);
        return (1);                    // Retorna 1 = "pule esta linha"
    }
    if (has_unclosed_quotes(line))     // Se tem aspas não fechadas
    {
        ft_putendl_fd("minishell: syntax error: unclosed quotes", 2);
        shell->exit_code = 2;
        free(line);
        return (1);
    }
    return (0);                        // Retorna 0 = "linha válida"
}
```

**Funções auxiliares (static):**
- `is_comment_line()` — pula espaços e verifica se o primeiro caractere significativo é `#`.
- `has_unclosed_quotes()` — percorre a string procurando pares de aspas. Se encontra uma aspa sem fechamento, retorna 1.

**Retorno:** `1` = linha inválida (já foi liberada, o loop faz `continue`). `0` = linha OK.

---

### 4.5 Processamento da Linha — `process_line()`

> 📍 `src/utils/main_utils.c`

```c
void process_line(char *line, t_shell *shell)
{
    t_token *tokens;
    t_cmd   *cmds;

    tokens = lexer(line, shell);       // ETAPA 1: Tokenização
    if (tokens)
    {
        shell->s_tokens = tokens;
        cmds = parser(tokens, shell);  // ETAPA 2: Parsing
        if (cmds)
        {
            shell->s_cmds = cmds;
            executor(cmds, shell);     // ETAPA 3: Execução
            free_cmds(cmds);
            shell->s_cmds = NULL;
        }
        free_tokens(tokens);
        shell->s_tokens = NULL;
    }
}
```

Esta função é o **núcleo do pipeline** — recebe a linha já validada e executa as três etapas principais em sequência: tokenização (lexer), análise sintática (parser) e execução (executor). Após cada etapa, as referências são guardadas em `shell->s_tokens` e `shell->s_cmds` para permitir limpeza de memória em caso de erro ou sinal.

> ⚠️ **Nota:** Esta implementação não tem suporte para separação por `;` (ponto e vírgula). Cada linha digitada é tratada como um único pipeline.

---

### 4.6 Lexer (Tokenização)

> 📍 `src/lexer/lexer.c`, `src/lexer/lexer_utils.c`, `src/lexer/lexer_word.c`

O **Lexer** é como um **cortador de texto** — pega a string crua e corta em pedaços significativos (tokens).

**Função principal: `lexer()`** (`src/lexer/lexer.c`)

```c
t_token *lexer(char *line, t_shell *shell)
{
    t_token *tokens;
    int     i;

    if (!line)
        return (NULL);
    tokens = NULL;
    i = 0;
    while (line[i])
    {
        process_char(&tokens, line, &i, shell);
        i++;
    }
    return (tokens);
}
```

**`process_char()`** decide o que fazer com cada caractere:

| Caractere | Ação | Token criado |
|-----------|------|-------------|
| Espaço/tab/newline | Ignora (pula) | Nenhum |
| `\|` | Cria token pipe | `TK_PIPE` com value `"\|"` |
| `<` | Verifica se é `<` ou `<<` | `TK_REDIR_IN` ou `TK_HEREDOC` |
| `>` | Verifica se é `>` ou `>>` | `TK_REDIR_OUT` ou `TK_APPEND` |
| Qualquer outro | Chama `build_full_word()` | `TK_WORD` com o texto |

---

#### Construção de palavras — `build_full_word()` (`src/lexer/lexer_word.c`)

Esta função constrói uma "palavra completa" juntando partes de diferentes tipos:

```c
char *build_full_word(char *line, int *i, t_shell *shell, int *quoted)
{
    char *result;
    char *part;
    int  had_quotes;

    result = ft_strdup("");
    *quoted = 0;
    had_quotes = (line[*i] == '\'' || line[*i] == '"');
    while (line[*i] && !is_end_of_word(line[*i]))
    {
        part = handle_word_part(line, i, shell, quoted);
        result = join_and_free(result, part);
    }
    *quoted = had_quotes;
    return (result);
}
```

**`handle_word_part()`** trata cada segmento da palavra:
- **Aspas simples** (`'...'`) → `extract_quoted_content()` → copiado **literalmente** (SEM expansão de `$VAR`)
- **Aspas duplas** (`"..."`) → `extract_quoted_content()` → depois passa por `expand_vars()` (COM expansão)
- **Texto sem aspas** → `extract_unquoted()` → depois passa por `expand_vars()`

O flag `had_quotes` é definido com base no primeiro caractere da palavra. Se começou com aspas, `no_expand` é marcado como `1` no token resultante (usado para bloquear expansão posterior no parser).

**Funções em `lexer_utils.c`:**
- `is_space(c)` — retorna 1 se `c` é `' '`, `'\t'` ou `'\n'`
- `is_special(c)` — retorna 1 se `c` é `'|'`, `'<'` ou `'>'`
- `new_token(tk_str, type)` — aloca e inicializa um novo token
- `token_add_back(tokens, new_node)` — adiciona um token ao final da lista encadeada
- `join_and_free(s1, s2)` — junta duas strings e liberta as originais

**Funções em `lexer_word.c`:**
- `is_end_of_word(c)` — retorna 1 se `c` é fim de string, espaço ou caractere especial
- `extract_quoted_content(line, i)` — extrai conteúdo entre aspas (avança `i` para depois da aspa de fecho)
- `extract_unquoted(line, i)` — extrai texto sem aspas até encontrar espaço ou especial
- `handle_word_part(line, i, shell, quoted)` — decide o tipo de segmento e retorna parte expandida
- `build_full_word(line, i, shell, quoted)` — monta a palavra completa juntando todos os segmentos

**Exemplo de tokenização:**
```
Input: echo "Hello $USER" | cat -e
         ↓
Tokens: [WORD:"echo"] → [WORD:"Hello joao"] → [PIPE:"|"] → [WORD:"cat"] → [WORD:"-e"]
```

---

### 4.7 Parser (Construção de Comandos)

> 📍 `src/parser/parser.c`, `src/parser/parser_utils.c`, `src/parser/parser_redir.c`

O **Parser** pega a lista de tokens e monta a **lista de comandos**. É como montar blocos de LEGO — os tokens são as peças individuais, e o parser monta a estrutura final.

**Função principal: `parser()`** (`src/parser/parser.c`)

```c
t_cmd *parser(t_token *tokens, t_shell *shell)
{
    t_cmd *cmds;
    t_cmd *current_cmd;

    if (!tokens)
        return (NULL);
    if (validate_syntax(tokens) != 0)  // 1. Valida a sintaxe
    {
        shell->exit_code = 2;
        return (NULL);
    }
    cmds = NULL;
    current_cmd = new_cmd();           // 2. Cria o primeiro comando
    cmd_add_back(&cmds, current_cmd);
    while (tokens)                     // 3. Percorre todos os tokens
    {
        process_token(&cmds, &current_cmd, &tokens);
        tokens = tokens->next;
    }
    return (cmds);
}
```

**Validação de sintaxe — `validate_syntax()`:**

Verifica erros como:
- `| ls` → pipe no início
- `ls ||` → pipe duplo
- `ls >` → redirecionamento sem alvo
- `> >` → redirecionamento seguido de redirecionamento

**`process_token()`** decide o que fazer com cada token:

| Tipo do Token | Ação |
|--------------|------|
| `TK_WORD` | Adiciona como argumento do comando atual via `cmd_add_arg()` (se não for string vazia, ou se vier de aspas) |
| `TK_PIPE` | Cria um **novo comando** e adiciona à lista |
| `TK_REDIR_IN` ou `TK_HEREDOC` | Chama `parse_redir_in()` |
| `TK_REDIR_OUT` ou `TK_APPEND` | Chama `parse_redir_out()` |

**Funções de suporte em `parser_utils.c`:**
- `new_cmd()` — Aloca um novo `t_cmd` com tudo zerado/NULL e `heredoc_fd = -1`
- `cmd_add_back()` — Adiciona comando ao final da lista encadeada
- `argv_len()` — (static) conta argumentos no array
- `cmd_add_arg()` — Adiciona argumento ao array `args` do comando (realoca o array)
- `redir_add_back()` — Adiciona redirecionamento ao final da lista

**Redirecionamentos em `parser_redir.c`:**

```c
void parse_redir_in(t_cmd *current_cmd, t_token **tokens)
{
    // Determina o tipo (REDIR_IN ou REDIR_HEREDOC)
    // Para heredoc com no_expand=1, envolve o delimitador em aspas simples
    // via quoted_target() para sinalizar que não deve expandir variáveis
    ...
}
```

- `quoted_target()` — (static) envolve o delimitador em `'...'` para sinalizar ausência de expansão
- `new_redir()` — (static) aloca e inicializa uma `t_redir`
- `parse_redir_in()` — Processa `<` e `<<`
- `parse_redir_out()` — Processa `>` e `>>`

**Exemplo de parsing:**
```
Tokens: [echo] → [Hello] → [|] → [cat] → [-e] → [>] → [out.txt]

Resultado:
CMD 1: args={"echo", "Hello"}, redirs=NULL, next → CMD 2
CMD 2: args={"cat", "-e"}, redirs→[REDIR_OUT, "out.txt"], next=NULL
```

---

### 4.8 Expander (Expansão de Variáveis)

> 📍 `src/expander/expander.c`

A **expansão** acontece durante a construção de palavras no lexer (em `build_full_word`) e no heredoc. Transforma `$VAR` no valor real da variável.

**Função principal: `expand_vars()`**

```c
char *expand_vars(char *str, char **env, int last_exit)
```

**Argumentos:**
- `str` (`char *`) — A string a expandir (ex: `"Hello $USER"`)
- `env` (`char **`) — O array de variáveis de ambiente
- `last_exit` (`int`) — O exit code do último comando (para `$?`)

**Retorno:** Uma nova string com as variáveis substituídas pelos seus valores.

**Como funciona:** Percorre a string caractere a caractere:
- Se encontra `$` seguido de algo → chama `extract_and_expand_var()`
- Senão → copia o caractere literal

**Casos especiais em `extract_and_expand_var()`:**
| Input | Resultado | Explicação |
|-------|-----------|------------|
| `$?` | `"0"` (ou o exit code) | Código de saída do último comando |
| `$$` | `"12345"` (PID do processo) | ID do processo via `getpid()` |
| `$HOME` | `"/home/user"` | Valor da variável HOME |
| `$NAOEXISTE` | `""` | Variável não definida → string vazia |
| `$1` | `""` | Parâmetro posicional → ignorado |
| `$` (sozinho no final) | `"$"` | Literal |

A busca do valor é feita por `get_env_value()` em `src/env/env_get.c`.

**Funções internas:**
- `get_var_value(var_name, env)` — busca o valor no array de env e retorna cópia
- `expand_var_name(str, i, env)` — lê o nome da variável (alfanumérico+underscore) e retorna o valor expandido
- `extract_and_expand_var(str, i, env, last_exit)` — trata casos especiais (`$?`, `$$`) e delega para `expand_var_name`
- `expand_vars(str, env, last_exit)` — função pública, percorre a string completa
- `expand_tokens(tokens, env, last_exit)` — itera a lista de tokens e expande os que têm `no_expand == 0`

---

### 4.9 Heredoc

> 📍 `src/heredoc/heredoc.c`, `src/heredoc/heredoc_utils.c`

O **heredoc** (`<<`) permite fornecer múltiplas linhas de input a um comando até encontrar um delimitador.

**Exemplo:**
```bash
cat << EOF
Olá mundo
Isto é um heredoc
EOF
```

**Função principal: `handle_heredoc()`** (`src/heredoc/heredoc.c`)

```c
int handle_heredoc(char *delimiter, char **env, int last_exit)
```

**Fluxo:**
1. Verifica se o delimitador tem aspas (`heredoc_has_quotes()`) → se sim, NÃO expande variáveis (`ctx.expand = 0`)
2. Remove aspas do delimitador (`heredoc_remove_quotes()`)
3. Gera um nome de ficheiro temporário (`heredoc_gen_temp_filename()`) → `.heredoc_tmp_0`, `.heredoc_tmp_1`, etc.
4. Abre o ficheiro para escrita
5. Configura sinais para modo heredoc (`setup_signals_heredoc()`)
6. Lê linhas até encontrar o delimitador (ou `Ctrl+C`/`Ctrl+D`) via `read_heredoc_lines()`
7. Se `expand` é 1, expande variáveis em cada linha com `expand_vars()`
8. Fecha, reabre o ficheiro para leitura, apaga o ficheiro temporário (`unlink`)
9. Retorna o fd

**`process_heredocs()`** — percorre todos os comandos da lista e para cada redirecionamento do tipo `REDIR_HEREDOC`, chama `handle_heredoc()` e armazena o fd em `cmd->heredoc_fd`.

**Funções utilitárias em `heredoc_utils.c`:**
- `heredoc_has_quotes(delimiter)` — verifica se o delimitador contém aspas simples ou duplas
- `heredoc_remove_quotes(delimiter)` — remove aspas envolventes do delimitador
- `heredoc_gen_temp_filename()` — gera nome único usando contador estático
- `heredoc_eof_warning(delimiter)` — imprime aviso quando o input termina sem o delimitador
- `heredoc_read_line()` — lê uma linha de stdin caractere a caractere (para modo não-TTY)

---

### 4.10 Executor

> 📍 `src/exec/execute.c`, `src/exec/child.c`, `src/exec/redirs.c`, `src/exec/path.c`, `src/exec/exec_errors.c`

O **executor** é o "motor" que faz os comandos realmente acontecerem.

**Função principal: `executor()`** (`src/exec/execute.c`)

```c
void executor(t_cmd *cmd, t_shell *shell)
{
    if (!cmd)
        return;
    process_heredocs(cmd, shell->env_vars, shell->exit_code);
    if (!cmd->next && cmd->args && is_right_assignment(cmd->args[0]))
    {
        update_env(cmd->args[0], &shell->env_vars);  // VAR=valor direto
        shell->exit_code = 0;
        return;
    }
    else if (!cmd->next && cmd->args && is_builtin(cmd->args))
        exec_single_builtin(cmd, shell);             // Builtin solo (sem fork)
    else
        execute_pipe(cmd, shell);                    // Pipeline (com fork)
}
```

O executor primeiro processa todos os heredocs, depois decide o caminho de execução:

```
executor(cmd, shell)
    ├── É uma atribuição direta (VAR=valor)?
    │   └── SIM → update_env() [sem fork, sem exec]
    ├── É builtin E não tem pipe?
    │   └── SIM → exec_single_builtin() [no pai, sem fork]
    └── NÃO → execute_pipe() [com fork]
```

---

#### Builtin Solo — `exec_single_builtin()`

```c
static void exec_single_builtin(t_cmd *cmd, t_shell *shell)
{
    int tmp_stdin;
    int tmp_stdout;

    tmp_stdin = dup(STDIN_FILENO);     // 1. Salva os FDs originais
    tmp_stdout = dup(STDOUT_FILENO);
    if (handle_redirection(cmd) != 0)  // 2. Aplica redirecionamentos
        shell->exit_code = 1;
    else
        shell->exit_code = exec_builtin(cmd, shell); // 3. Executa o builtin
    dup2(tmp_stdin, STDIN_FILENO);     // 4. Restaura os FDs originais
    dup2(tmp_stdout, STDOUT_FILENO);
    close(tmp_stdin);
    close(tmp_stdout);
}
```

Builtins como `cd` e `export` **precisam** rodar no processo pai (alteram o estado do shell). Por isso não podem ser executados num fork.

---

#### Pipeline — `execute_pipe()`

```c
void execute_pipe(t_cmd *cmd, t_shell *shell)
{
    int   fd_in;
    pid_t pid;

    pid = -1;
    fd_in = -1;
    setup_signals_execution();
    pipe_loop(cmd, &fd_in, &pid, shell);
    if (fd_in != -1)
        close(fd_in);
    wait_children(pid, shell);
    setup_signals();
}
```

**`pipe_loop()`** — Para cada comando na lista:
1. Se tem próximo → cria um `pipe(fd_pipe)` (canal de comunicação)
2. `fork()` → cria um processo filho
3. No **filho** (`pid == 0`) → `child_process()`
4. No **pai** → fecha fd de escrita, guarda fd de leitura para o próximo ciclo

**`wait_children()`** — Aguarda todos os filhos terminarem. O exit code final é do último processo. Trata `SIGQUIT` (imprime "Quit (core dump)") e `SIGINT` (imprime newline).

---

#### Processo Filho — `child_process()`

> 📍 `src/exec/child.c`

```c
void child_process(t_cmd *cmd, int fd_in, int *fd_pipe, t_shell *shell)
{
    signal(SIGINT, SIG_DFL);           // Restaura sinais padrão no filho
    signal(SIGQUIT, SIG_DFL);
    handle_pipes(cmd, fd_in, fd_pipe); // Redireciona stdin/stdout para pipes
    if (handle_redirection(cmd) != 0)  // Aplica redirecionamentos de ficheiro
        cleanup_exit_child(shell, 1);
    child_exec(cmd, shell);            // Decide se é builtin ou execve
}
```

**`handle_pipes()`** — Conecta os pipes:
- Se `fd_in != -1` → `dup2(fd_in, STDIN)` (lê do pipe anterior)
- Se tem próximo comando → `dup2(fd_pipe[1], STDOUT)` (escreve para o pipe seguinte)

**`child_exec()`** — Decide como executar:
- Se é builtin → `exec_builtin()` → `cleanup_exit_child()`
- Se é comando externo → `handle_exec_path()` → `execve()`

**`resolve_cmd_path()`** (static) — Resolve o caminho do executável:
- Se tem `/` → verifica acesso direto
- Senão → chama `find_path()` para buscar no PATH

---

#### Busca do PATH — `find_path()`

> 📍 `src/exec/path.c`

```c
char *find_path(char *cmd, char **envp)
```

**Lógica:**
1. Se `cmd` contém `/` → verifica se existe e é executável
2. Senão → procura `PATH=` nas variáveis, divide por `:`, testa cada diretório via `search_path()`

---

#### Redirecionamentos — `handle_redirection()`

> 📍 `src/exec/redirs.c`

Percorre a lista de `t_redir` do comando e para cada uma:
| Tipo | Ação |
|------|------|
| `REDIR_IN` (`<`) | `open(file, O_RDONLY)` → `dup2(fd, STDIN)` |
| `REDIR_OUT` (`>`) | `open(file, O_WRONLY\|O_CREAT\|O_TRUNC, 0644)` → `dup2(fd, STDOUT)` |
| `REDIR_APPEND` (`>>`) | `open(file, O_WRONLY\|O_CREAT\|O_APPEND, 0644)` → `dup2(fd, STDOUT)` |
| `REDIR_HEREDOC` (`<<`) | `dup2(cmd->heredoc_fd, STDIN)` |

---

#### Erros de Execução — `execution_error()`

> 📍 `src/exec/exec_errors.c`

| Situação | Código | Mensagem |
|----------|--------|----------|
| Comando com `/` que é diretório | 126 | "Is a directory" |
| Comando com `/` que não existe | 127 | "No such file or directory" |
| Comando sem `/` não encontrado no PATH | 127 | "command not found" |
| Sem permissão de execução | 126 | "Permission denied" |

`print_error_message()` (static) constrói a mensagem completa com `ft_strjoin` e envia numa única chamada `write()` para o fd 2, evitando intercalação de mensagens entre processos.

`is_right_assignment()` verifica se um argumento é uma atribuição de variável válida (ex: `FOO=bar`).

---

### 4.11 Builtins

> 📍 `src/builtins/`

#### Router — `is_builtin()` e `exec_builtin()`

> 📍 `src/builtins/builtins_router.c`

`is_builtin()` verifica se `args[0]` é um dos 7 builtins. Usa `ft_strncmp` com o tamanho `strlen + 1` para garantir correspondência exata. Note que `env` apenas é considerado builtin quando não tem argumentos (`args[1] == NULL`).

`exec_builtin()` redireciona para a função correta conforme o nome do comando.

---

#### `echo` — `ft_echo()`

> 📍 `src/builtins/builtins_info.c`

- Verifica a flag `-n` (e variantes como `-nnn`, `-nnnn`) via `is_valid_n_flag()`.
- Imprime todos os argumentos separados por espaço.
- Se NÃO tem `-n` → imprime newline no final via `ft_putendl_fd("", 1)`.

---

#### `pwd` — `ft_pwd()`

> 📍 `src/builtins/builtins_info.c`

- Primeiro tenta `get_env_value(shell->env_vars, "PWD")`.
- Se falhar ou estiver vazio, usa `getcwd(buf, PATH_MAX)`.

---

#### `cd` — `ft_cd()`

> 📍 `src/builtins/builtin_cd.c`

- `cd` sem argumentos ou `cd ~` → vai para `$HOME`.
- `cd -` → vai para `$OLDPWD` e imprime o caminho.
- `cd <path>` → `chdir(path)`.
- Atualiza `OLDPWD` e `PWD` via `update_w_export()` que chama `ft_export`.
- Caso o `getcwd()` falhe (diretório apagado), constrói o caminho manualmente concatenando o caminho anterior com o destino.

---

#### `exit` — `ft_exit()`

> 📍 `src/builtins/builtin_exit.c`

- Imprime "exit".
- Suporta o argumento especial `--` (ignora e avança).
- Sem argumentos → sai com `shell->exit_code`.
- Com argumento numérico → sai com `(unsigned char)res`.
- Argumento não numérico → `exit_numeric_error()` → exit code 2.
- Muitos argumentos → erro, NÃO sai (exit code 1).

---

#### `export` — `ft_export()`

> 📍 `src/builtins/builtins_env.c`

- Sem argumentos → `show_export_list()` (imprime variáveis ordenadas no formato `declare -x`).
- Com argumentos → valida a chave (`is_valid_key()`) e adiciona/atualiza via `update_env()`.
- Suporta `+=` para concatenar valores (tratado em `exp_and_append()`).

---

#### `unset` — `ft_unset()`

> 📍 `src/builtins/builtins_env.c`

Remove variáveis do ambiente. Cria um novo array sem a variável indicada, usando `is_key_match()` para verificar correspondência exata (incluindo caso sem `=`).

---

#### `env` — `ft_env()`

> 📍 `src/builtins/builtins_env.c`

Imprime todas as variáveis de ambiente que contêm `=` (variáveis com valor atribuído).

---

### 4.12 Sinais

> 📍 `src/signals/signals.c`

A **única variável global** do projeto:
```c
int g_last_signal = 0;  // Armazena o último sinal recebido
```

**3 contextos de sinais:**

| Contexto | SIGINT (Ctrl+C) | SIGQUIT (Ctrl+\) | Função |
|----------|-----------------|-------------------|--------|
| **Prompt** (esperando input) | Nova linha, limpa input, redesenha prompt | Ignorado | `setup_signals()` |
| **Execução** (filhos rodando) | Ignorado no pai, default nos filhos | Ignorado no pai, default nos filhos | `setup_signals_execution()` |
| **Heredoc** (lendo input do heredoc) | Nova linha, fecha STDIN (aborta heredoc) | Ignorado | `setup_signals_heredoc()` |

**`handle_sigint()`** (para o prompt):
```c
void handle_sigint(int sig)
{
    g_last_signal = sig;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
```

**`handle_sigint_heredoc()`** (static, para o heredoc):
```c
static void handle_sigint_heredoc(int sig)
{
    g_last_signal = sig;
    write(1, "\n", 1);
    close(STDIN_FILENO);  // Fecha stdin para abortar o readline do heredoc
}
```

Todas as funções de setup usam `sigaction` com `struct sigaction` (zero-inicializado com `ft_memset`) para maior portabilidade e controlo.

---

### 4.13 Limpeza de Memória

> 📍 `src/utils/free.c`

**`free_all()`** — Liberta TUDO (usado no `exit` e em erros fatais):
- `free_tokens(shell->s_tokens)` → Liberta lista de tokens (valor + nó)
- `free_cmds(shell->s_cmds)` → Liberta lista de comandos (args, redirs, fecha heredoc_fd)
- `free_env(shell->env_vars)` → Liberta array de variáveis de ambiente
- `rl_clear_history()` → Limpa o histórico do readline

**`cleanup_exit_child()`** — Versão para processos filhos: liberta tokens, cmds e env, mas NÃO chama `rl_clear_history()`, e termina com `exit(exit_code)`.

**`free_cmds()`** — Para cada comando: liberta todos os strings em `args`, chama `free_redirs()` para a lista de redirecionamentos, e fecha `heredoc_fd` se estiver aberto (≥ 0).

---

## 5. 📊 Diagrama do Fluxo

```
╔════════════════════════════════════════════════════════════════╗
║                          main()                                ║
║  src/main.c                                                    ║
║  init_shell() → shlvl_update() → setup_signals()              ║
╚═════════════════════╦══════════════════════════════════════════╝
                      ▼
         ┌─────── shell_loop() ◄─────────────────────┐
         │    src/main.c                              │
         ▼                                            │
 ┌───────────────┐                                    │
 │  get_input()  │  readline("minishell> ")           │
 │  src/main.c   │  ou read_line()                    │
 └───────┬───────┘                                    │
         │ line                                       │
         ▼                                            │
 ┌─────────────────────┐  inválida                    │
 │   validate_line()   │──────── free(line) ──────────┤
 │  main_utils.c       │  (aspas, #)                  │
 └───────┬─────────────┘                              │
         │ válida                                     │
         ▼                                            │
 ┌─────────────────────┐                              │
 │   process_line()    │                              │
 │  main_utils.c       │                              │
 └───────┬─────────────┘                              │
         │                                            │
 ┌───────▼───────┐  ┌──────────────┐  ┌────────────────────┐
 │    lexer()    │→ │   parser()   │→ │    executor()      │
 │  lexer.c      │  │  parser.c    │  │  execute.c         │
 └───────────────┘  └──────────────┘  └────────┬───────────┘
                                               │
                                    process_heredocs()
                                               │
                                    ┌──────────┼──────────┐
                                    ▼          ▼          ▼
                            ┌──────────┐  ┌─────────┐  ┌──────────────┐
                            │ VAR=val  │  │ Builtin │  │ execute_pipe │
                            │ update   │  │ solo    │  │ (com fork)   │
                            │ env      │  │(no pai) │  └──────┬───────┘
                            └──────────┘  └─────────┘         │
                                                     ┌─────────▼─────────┐
                                                     │  child_process()  │
                                                     │    child.c        │
                                                     └─────────┬─────────┘
                                                               │
                                                     ┌─────────▼─────────┐
                                                     │ handle_pipes()    │
                                                     │ handle_redir()    │
                                                     │ child_exec()      │
                                                     │   → execve()      │
                                                     └───────────────────┘
                                                               │
         ┌─────────────────────────────────────────────────────┘
         │ free(line), voltar ao loop
         └──────────────────────────────────────────────────────►┘
```

---

## 6. 🔍 Análise de Norminette

A norma da 42 (norminette) impõe regras rígidas de estilo. Abaixo está a análise de cada módulo:

### Regras verificadas:
- ✅ **Máximo 25 linhas por função** (corpo, sem contar chaves)
- ✅ **Máximo 5 funções por arquivo `.c`** (excluindo `main`)
- ✅ **Máximo 4 parâmetros por função**
- ✅ **Apenas 1 variável global** (`g_last_signal` — permitida pelo subject)
- ✅ **Header 42** em todos os ficheiros
- ✅ **Prefixos corretos**: `s_` para structs, `t_` para typedefs, `e_` para enums
- ✅ **Declarações de variáveis no topo** de cada função
- ✅ **Indentação com tabs**

### Resultado por arquivo:

| Arquivo | Funções | Status | Funções |
|---------|---------|--------|---------|
| `src/main.c` | 4 | ✅ OK | `get_input`(s), `shell_loop`(s), `init_shell`(s), `main` |
| `src/lexer/lexer.c` | 4 | ✅ OK | `handle_redir_out`(s), `handle_redir_in`(s), `process_char`(s), `lexer` |
| `src/lexer/lexer_utils.c` | 5 | ✅ OK | `is_space`, `is_special`, `new_token`, `token_add_back`, `join_and_free` |
| `src/lexer/lexer_word.c` | 5 | ✅ OK | `is_end_of_word`(s), `extract_quoted_content`(s), `extract_unquoted`(s), `handle_word_part`(s), `build_full_word` |
| `src/parser/parser.c` | 5 | ✅ OK | `check_pipe_syntax`(s), `check_redir_syntax`(s), `validate_syntax`(s), `process_token`(s), `parser` |
| `src/parser/parser_utils.c` | 5 | ✅ OK | `new_cmd`, `cmd_add_back`, `argv_len`(s), `cmd_add_arg`, `redir_add_back` |
| `src/parser/parser_redir.c` | 4 | ✅ OK | `quoted_target`(s), `new_redir`(s), `parse_redir_in`, `parse_redir_out` |
| `src/expander/expander.c` | 5 | ✅ OK | `get_var_value`(s), `expand_var_name`(s), `extract_and_expand_var`(s), `expand_vars`, `expand_tokens` |
| `src/heredoc/heredoc.c` | 5 | ✅ OK | `write_hd_line`(s), `read_heredoc_lines`(s), `heredoc_read`(s), `handle_heredoc`, `process_heredocs` |
| `src/heredoc/heredoc_utils.c` | 5 | ✅ OK | `heredoc_has_quotes`, `heredoc_remove_quotes`, `heredoc_gen_temp_filename`, `heredoc_eof_warning`, `heredoc_read_line` |
| `src/exec/execute.c` | 5 | ✅ OK | `exec_single_builtin`(s), `wait_children`(s), `pipe_loop`(s), `execute_pipe`, `executor` |
| `src/exec/child.c` | 5 | ✅ OK | `resolve_cmd_path`(s), `handle_exec_path`(s), `handle_pipes`, `child_exec`(s), `child_process` |
| `src/exec/redirs.c` | 3 | ✅ OK | `apply_redir`(s), `apply_heredoc`(s), `handle_redirection` |
| `src/exec/path.c` | 2 | ✅ OK | `search_path`(s), `find_path` |
| `src/exec/exec_errors.c` | 3 | ✅ OK | `print_error_message`(s), `is_right_assignment`, `execution_error` |
| `src/builtins/builtins_router.c` | 2 | ✅ OK | `is_builtin`, `exec_builtin` |
| `src/builtins/builtins_info.c` | 3 | ✅ OK | `is_valid_n_flag`, `ft_echo`, `ft_pwd` |
| `src/builtins/builtin_cd.c` | 5 | ✅ OK | `print_cd_error`(s), `update_w_export`(s), `get_target_dir`(s), `manage_pwd`(s), `ft_cd` |
| `src/builtins/builtin_exit.c` | 2 | ✅ OK | `exit_numeric_error`(s), `ft_exit` |
| `src/builtins/builtins_env.c` | 5 | ✅ OK | `is_key_match`(s), `remove_env_var`(s), `ft_unset`, `ft_export`, `ft_env` |
| `src/builtins/export_print.c` | 3 | ✅ OK | `print_export_line`(s), `sort_env_matrix`(s), `show_export_list` |
| `src/env/env_init.c` | 4 | ✅ OK | `shlvl_update`, `count_env`(s), `copy_env`, `free_env` |
| `src/env/env_get.c` | 2 | ✅ OK | `get_matrix_len`, `get_env_value` |
| `src/env/env_modify.c` | 4 | ✅ OK | `is_valid_key`, `add_var_to_env`, `exp_and_append`(s), `update_env` |
| `src/signals/signals.c` | 5 | ✅ OK | `handle_sigint`, `setup_signals`, `setup_signals_execution`, `handle_sigint_heredoc`(s), `setup_signals_heredoc` |
| `src/utils/free.c` | 5 | ✅ OK | `free_all`, `free_tokens`, `free_redirs`(s), `free_cmds`, `cleanup_exit_child` |
| `src/utils/main_utils.c` | 4 | ✅ OK | `is_comment_line`(s), `has_unclosed_quotes`(s), `validate_line`, `process_line` |
| `src/utils/utils.c` | 4 | ✅ OK | `special_expand_params`, `ft_strcmp`, `free_tab`, `ft_isspace` |
| `src/utils/utils2.c` | 5 | ✅ OK | `parse_sign_and_skip`(s), `overflow_check`(s), `ft_atoll_overflow`, `append_char`(s), `read_line` |
| `src/utils/debug.c` | 3 | ✅ OK | `print_tokens`, `print_redir`(s), `print_cmds` |

*(s) = função static*

---

### ✅ Análise Final — Sem Violações Detectadas

Ao contrário de versões anteriores do código, a refatoração atual apresenta conformidade total com a norminette nos seguintes pontos:

**`src/utils/main_utils.c` — 4 funções ✅**

O arquivo foi simplificado e contém apenas:
1. `is_comment_line()` (static)
2. `has_unclosed_quotes()` (static)
3. `validate_line()`
4. `process_line()`

> ℹ️ A função `process_line()` chama diretamente `lexer()` → `parser()` → `executor()`. **Não existe** separação por `;` nesta implementação — cada linha digitada é um único pipeline completo.

**Pontos a verificar com `norminette` local:**

| Critério | Resultado |
|----------|-----------|
| Header 42 em todos os ficheiros | ✅ Presente |
| Máx 25 linhas por função | ✅ Todas OK |
| Máx 5 funções por `.c` | ✅ Todas OK |
| Máx 4 parâmetros | ✅ Todas OK — `child_process(cmd, fd_in, fd_pipe, shell)` e `build_full_word(line, i, shell, quoted)` têm exatamente 4 |
| 1 variável global (permitida) | ✅ Apenas `g_last_signal` |
| Prefixos `s_`, `t_`, `e_` | ✅ Corretos |
| Declarações no topo | ✅ Corretas |
| Indentação com tabs | ✅ Correta |
| Máx 80 colunas | ⚠️ Verificar com norminette — linhas longas em `ft_putendl_fd` e alguns comentários podem estar no limite |

> **📝 Nota:** Esta documentação foi atualizada com base na análise direta do código-fonte. A versão anterior continha informações incorretas sobre `next_semicolon()` e `exec_single()` — funções que **não existem** nesta implementação.
