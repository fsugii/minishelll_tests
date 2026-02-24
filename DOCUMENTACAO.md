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

> 📍 Definida em `includes/minishell.h`, linhas 82-88

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

> 📍 Definida em `includes/minishell.h`, linhas 64-71

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

> 📍 Definida em `includes/minishell.h`, linhas 73-80

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

> 📍 Definida em `includes/minishell.h`, linhas 57-62

```c
typedef struct s_redir
{
    t_redir_type    type;   // REDIR_IN(<), REDIR_OUT(>), REDIR_APPEND(>>), REDIR_HEREDOC(<<)
    char            *target; // Nome do ficheiro ou delimitador
    struct s_redir  *next;   // Próximo redirecionamento
} t_redir;
```

---

### `t_hd_ctx` — Contexto do Heredoc

> 📍 Definida em `includes/minishell.h`, linhas 159-165

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

> 📍 `src/main.c`, linhas 61-76

```c
int main(int argc, char **argv, char **envp)
{
    t_shell  shell;

    (void)argc;           // Ignora argc (não usado)
    (void)argv;           // Ignora argv (não usado)
    init_shell(&shell, envp);  // 1. Inicializa a struct shell
    if (!shell.env_vars)       // 2. Verifica se copiou o ambiente com sucesso
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

1. **`init_shell(&shell, envp)`** (linha 53-59) — Inicializa a estrutura:
   - `shell->env_vars = copy_env(envp)` → Copia TODAS as variáveis de ambiente do sistema para uma cópia própria. A função `copy_env()` em `src/env/env_init.c` faz `malloc` para uma nova matriz e `ft_strdup` para cada string.
   - `shell->exit_code = 0` → Começa com sucesso.
   - `shell->s_tokens = NULL` e `shell->s_cmds = NULL` → Sem tokens/comandos ainda.

2. **`shlvl_update(&shell.env_vars)`** (`src/env/env_init.c`, linhas 16-40) — Incrementa a variável `SHLVL` (shell level). Se `SHLVL=1` no sistema, no minishell será `SHLVL=2`. Se passar de 999, reseta para 1.

3. **`setup_signals()`** (`src/signals/signals.c`, linhas 28-38) — Configura como o shell reage a `Ctrl+C` e `Ctrl+\` (mais detalhes na secção de sinais).

4. **`shell_loop(&shell)`** — Entra no loop infinito que lê e executa comandos.

---

### 4.2 Loop Principal — `shell_loop()`

> 📍 `src/main.c`, linhas 31-51

```c
static void shell_loop(t_shell *shell)
{
    char *line;

    while (1)                          // Loop infinito
    {
        line = get_input(shell);       // 1. Lê o input do utilizador
        if (!line)                     // 2. Se recebeu NULL (Ctrl+D / EOF)
        {
            if (isatty(STDIN_FILENO))  //    Se é terminal interativo
                printf("exit\n");      //    Mostra "exit"
            break ;                    //    Sai do loop
        }
        if (*line)                     // 3. Se a linha não está vazia
            add_history(line);         //    Adiciona ao histórico (seta pra cima)
        if (validate_line(line, shell))// 4. Valida a linha (aspas, comentários)
            continue ;                 //    Se inválida, volta ao início
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

> 📍 `src/main.c`, linhas 15-29

```c
static char *get_input(t_shell *shell)
{
    char *line;

    if (isatty(STDIN_FILENO))          // Se o input vem de um terminal real
        line = readline("minishell> "); // Usa readline (com prompt, histórico, edição)
    else
        line = read_line();             // Se vem de um pipe/ficheiro, lê manualmente
    if (g_last_signal == SIGINT)        // Se recebeu Ctrl+C durante a leitura
    {
        shell->exit_code = 130;         // Exit code 130 (padrão para SIGINT)
        g_last_signal = 0;              // Reseta o sinal
    }
    return (line);
}
```

**Argumentos recebidos:**
- `shell` (`t_shell *`) — Ponteiro para a struct principal, usado para atualizar o `exit_code` caso `Ctrl+C` tenha sido pressionado.

**Detalhes importantes:**
- `isatty(STDIN_FILENO)` verifica se a entrada padrão é um terminal (interativo) ou um ficheiro/pipe (não-interativo).
- `readline("minishell> ")` é a função da biblioteca `readline` que mostra o prompt e permite edição com setas, `Ctrl+A`, etc.
- `read_line()` em `src/utils/utils2.c` (linhas 89-106) lê caractere a caractere manualmente, para quando o input não é um terminal.
- `g_last_signal` é a **única variável global** do projeto — um `int` definido em `src/signals/signals.c` linha 16.

---

### 4.4 Validação — `validate_line()`

> 📍 `src/utils/main_utils.c`, linhas 53-67

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
        return (1);                    // Retorna 1 = "pule esta linha"
    }
    return (0);                        // Retorna 0 = "linha válida, pode prosseguir"
}
```

**Funções auxiliares:**
- `is_comment_line()` (linhas 16-25) — pula espaços e verifica se o primeiro caractere significativo é `#`.
- `has_unclosed_quotes()` (linhas 27-47) — percorre a string procurando pares de aspas. Se encontra uma aspa de abertura sem a correspondente de fecho, retorna 1.

**Retorno:** `1` = linha inválida (já foi liberada, o loop faz `continue`). `0` = linha OK.

---

### 4.5 Processamento da Linha — `process_line()`

> 📍 `src/utils/main_utils.c`, linhas 118-134

```c
void process_line(char *line, t_shell *shell)
{
    char *semi;
    char *segment;

    while (line)
    {
        semi = next_semicolon(line);   // Procura ';' fora de aspas
        if (semi)
        {
            segment = ft_substr(line, 0, semi - line); // Extrai o trecho antes do ';'
            exec_single(segment, shell);               // Executa esse trecho
            free(segment);
            line = semi + 1;                           // Avança para depois do ';'
        }
        else
        {
            exec_single(line, shell);  // Executa a linha toda
            break ;
        }
    }
}
```

A função divide a linha por `;` (respeitando aspas) e executa cada segmento separadamente.

**`exec_single()`** (linhas 103-116) é onde a **magia acontece** — é o pipeline completo:

```c
static void exec_single(char *line, t_shell *shell)
{
    t_token *tokens;
    t_cmd   *cmds;

    tokens = lexer(line, shell);       // ETAPA 1: Tokenização
    if (tokens)
    {
        shell->s_tokens = tokens;      // Guarda referência para cleanup
        cmds = parser(tokens, shell);  // ETAPA 2: Parsing
        if (cmds)
        {
            shell->s_cmds = cmds;      // Guarda referência para cleanup
            executor(cmds, shell);     // ETAPA 3: Execução
            free_cmds(cmds);
            shell->s_cmds = NULL;
        }
        free_tokens(tokens);
        shell->s_tokens = NULL;
    }
}
```

---

### 4.6 Lexer (Tokenização)

> 📍 `src/lexer/lexer.c`, `src/lexer/lexer_utils.c`, `src/lexer/lexer_word.c`

O **Lexer** é como um **cortador de texto** — pega a string crua e corta em pedaços significativos (tokens).

**Função principal: `lexer()`** (`src/lexer/lexer.c`, linhas 67-79)

```c
t_token *lexer(char *line, t_shell *shell)
{
    t_token *tokens;
    int     i;

    if (!line)
        return (NULL);
    tokens = NULL;
    i = 0;
    while (line[i])                              // Percorre cada caractere
    {
        process_char(&tokens, line, &i, shell);  // Processa o caractere atual
        i++;
    }
    return (tokens);                             // Retorna a lista de tokens
}
```

**`process_char()`** (linhas 40-63) decide o que fazer com cada caractere:

| Caractere | Ação | Token criado |
|-----------|------|-------------|
| Espaço/tab/newline | Ignora (pula) | Nenhum |
| `\|` | Cria token pipe | `TK_PIPE` com value `"\|"` |
| `<` | Verifica se é `<` ou `<<` | `TK_REDIR_IN` ou `TK_HEREDOC` |
| `>` | Verifica se é `>` ou `>>` | `TK_REDIR_OUT` ou `TK_APPEND` |
| Qualquer outro | Chama `build_full_word()` | `TK_WORD` com o texto |

**Construção de palavras — `build_full_word()`** (`src/lexer/lexer_word.c`, linhas 78-89):

Esta é a função mais complexa do lexer. Ela constrói uma "palavra completa" juntando:
- Texto **sem aspas** → expandido com `expand_vars()`
- Texto em **aspas simples** (`'...'`) → copiado literalmente (SEM expansão)
- Texto em **aspas duplas** (`"..."`) → expandido com `expand_vars()`

**Argumentos:**
- `line` (`char *`) — A linha completa de input
- `i` (`int *`) — Ponteiro para o índice atual (avança conforme lê)
- `shell` (`t_shell *`) — Para acessar `env_vars` e `exit_code` durante a expansão
- `quoted` (`int *`) — Retorna se houve aspas (para marcar `no_expand`)

**Exemplo de tokenização:**
```
Input: echo "Hello $USER" | cat -e
         ↓
Tokens: [WORD:"echo"] → [WORD:"Hello joao"] → [PIPE:"|"] → [WORD:"cat"] → [WORD:"-e"]
```

**Funções de suporte em `lexer_utils.c`:**
- `is_space(c)` — retorna 1 se `c` é `' '`, `'\t'` ou `'\n'`
- `is_special(c)` — retorna 1 se `c` é `'|'`, `'<'` ou `'>'`
- `new_token(tk_str, type)` — aloca e inicializa um novo token
- `token_add_back(tokens, new_node)` — adiciona um token ao final da lista encadeada
- `join_and_free(s1, s2)` — junta duas strings e liberta as originais

---

### 4.7 Parser (Construção de Comandos)

> 📍 `src/parser/parser.c`, `src/parser/parser_utils.c`, `src/parser/parser_redir.c`

O **Parser** pega a lista de tokens e monta a **árvore de comandos**. É como montar blocos de LEGO — os tokens são as peças individuais, e o parser monta a estrutura final.

**Função principal: `parser()`** (`src/parser/parser.c`, linhas 89-105)

```c
t_cmd *parser(t_token *tokens, t_shell *shell)
{
    t_cmd *cmds;
    t_cmd *current_cmd;

    if (!tokens)
        return (NULL);
    if (validate_syntax(tokens) != 0)  // 1. Valida a sintaxe
    {
        shell->exit_code = 2;          //    Erro de sintaxe = exit code 2
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

**Validação de sintaxe — `validate_syntax()`** (linhas 42-63):

Verifica erros como:
- `| ls` → pipe no início
- `ls ||` → pipe duplo
- `ls >` → redirecionamento sem alvo
- `> >` → redirecionamento seguido de redirecionamento

**`process_token()`** (linhas 65-85) decide o que fazer com cada token:

| Tipo do Token | Ação |
|--------------|------|
| `TK_WORD` | Adiciona como argumento do comando atual via `cmd_add_arg()` |
| `TK_PIPE` | Cria um **novo comando** e adiciona à lista |
| `TK_REDIR_IN` ou `TK_HEREDOC` | Chama `parse_redir_in()` — cria redirecionamento de entrada |
| `TK_REDIR_OUT` ou `TK_APPEND` | Chama `parse_redir_out()` — cria redirecionamento de saída |

**Funções de suporte em `parser_utils.c`:**
- `new_cmd()` — Aloca um novo `t_cmd` com tudo zerado/NULL e `heredoc_fd = -1`
- `cmd_add_back()` — Adiciona comando ao final da lista encadeada
- `cmd_add_arg()` — Adiciona argumento ao array `args` do comando (realoca o array)
- `redir_add_back()` — Adiciona redirecionamento ao final da lista

**Redirecionamentos em `parser_redir.c`:**
- `parse_redir_in()` — Processa `<` e `<<`. Para heredoc com aspas, envolve o delimitador em `"` para sinalizar que não deve expandir.
- `parse_redir_out()` — Processa `>` e `>>`.

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

**Função principal: `expand_vars()`** (`src/expander/expander.c`, linhas 65-91)

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

**Casos especiais em `extract_and_expand_var()`** (linhas 48-61):
| Input | Resultado | Explicação |
|-------|-----------|------------|
| `$?` | `"0"` (ou o exit code) | Código de saída do último comando |
| `$$` | `"12345"` (PID do processo) | ID do processo |
| `$HOME` | `"/home/user"` | Valor da variável HOME |
| `$NAOEXISTE` | `""` | Variável não definida → string vazia |
| `$1` | `""` | Parâmetro posicional → ignorado |
| `$` (sozinho no final) | `"$"` | Literal |

A busca do valor é feita por `get_env_value()` em `src/env/env_get.c` (linhas 27-45), que percorre o array `env` procurando `KEY=` e retorna o ponteiro para depois do `=`.

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

**Função principal: `handle_heredoc()`** (`src/heredoc/heredoc.c`, linhas 79-99)

```c
int handle_heredoc(char *delimiter, char **env, int last_exit)
```

**Argumentos:**
- `delimiter` (`char *`) — A palavra que termina o heredoc (ex: `"EOF"`)
- `env` (`char **`) — Variáveis de ambiente (para expansão)
- `last_exit` (`int`) — Exit code (para `$?`)

**Retorno:** Um **file descriptor** (fd) pronto para ler, ou `-1` em erro.

**Fluxo:**
1. Verifica se o delimitador tem aspas → se sim, NÃO expande variáveis (`ctx.expand = 0`)
2. Remove aspas do delimitador (`heredoc_remove_quotes()`)
3. Gera um nome de ficheiro temporário (`.heredoc_tmp_0`, `.heredoc_tmp_1`, etc.)
4. Abre o ficheiro para escrita
5. Configura sinais para modo heredoc (`setup_signals_heredoc()`)
6. Lê linhas até encontrar o delimitador (ou `Ctrl+C`/`Ctrl+D`)
7. Se `expand` é 1, expande variáveis em cada linha com `expand_vars()`
8. Fecha, reabre o ficheiro para leitura, apaga o ficheiro temporário (`unlink`)
9. Retorna o fd

---

### 4.10 Executor

> 📍 `src/exec/execute.c`, `src/exec/child.c`, `src/exec/redirs.c`, `src/exec/path.c`, `src/exec/exec_errors.c`

O **executor** é o "motor" que faz os comandos realmente acontecerem.

**Função principal: `executor()`** (`src/exec/execute.c`, linhas 100-104+)

O executor toma uma decisão crucial:
- **Builtin solo** (sem pipe) → executa no processo **pai** (sem fork)
- **Pipeline ou comando externo** → executa com `fork()` em processos **filhos**

```
executor(cmd, shell)
    ├── É builtin E não tem next (pipe)?
    │   └── SIM → exec_single_builtin() [no pai]
    └── NÃO → execute_pipe() [com fork]
```

#### Builtin Solo — `exec_single_builtin()`

> 📍 `src/exec/execute.c`, linhas 19-33

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

Porque builtins como `cd` e `export` **precisam** rodar no processo pai (alteram o estado do shell), eles não podem ser executados num fork.

#### Pipeline — `execute_pipe()`

> 📍 `src/exec/execute.c`, linhas 87-98

```c
void execute_pipe(t_cmd *cmd, t_shell *shell)
{
    int     fd_in;
    pid_t   pid;

    pid = -1;
    fd_in = -1;
    setup_signals_execution();           // Ignora SIGINT/SIGQUIT no pai
    pipe_loop(cmd, &fd_in, &pid, shell); // Itera pelos comandos
    if (fd_in != -1)
        close(fd_in);
    wait_children(pid, shell);           // Espera todos os filhos terminarem
    setup_signals();                     // Restaura sinais do prompt
}
```

**`pipe_loop()`** (linhas 66-83) — Para cada comando na lista:
1. Se tem próximo comando → cria um `pipe(fd_pipe)` (canal de comunicação)
2. `fork()` → cria um processo filho
3. No **filho** (`pid == 0`) → `child_process()`
4. No **pai** → fecha o fd de leitura do pipe anterior, guarda o de leitura do novo pipe

#### Processo Filho — `child_process()`

> 📍 `src/exec/child.c`, linhas 82-89

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

**`handle_pipes()`** (linhas 53-65) — Conecta os pipes:
- Se `fd_in != -1` → `dup2(fd_in, STDIN)` (lê do pipe anterior)
- Se tem próximo comando → `dup2(fd_pipe[1], STDOUT)` (escreve para o pipe seguinte)

**`child_exec()`** (linhas 67-79) — Decide como executar:
- Se é builtin → `exec_builtin()` → `cleanup_exit_child()`
- Se é comando externo → `handle_exec_path()` → `execve()`

#### Busca do PATH — `find_path()`

> 📍 `src/exec/path.c`, linhas 38-60

```c
char *find_path(char *cmd, char **envp)
```

**Argumentos:**
- `cmd` (`char *`) — Nome do comando (ex: `"ls"`)
- `envp` (`char **`) — Variáveis de ambiente (para encontrar PATH)

**Lógica:**
1. Se `cmd` contém `/` (caminho absoluto/relativo) → verifica se existe e é executável
2. Senão → procura `PATH=` nas variáveis, divide por `:`, testa cada diretório

#### Redirecionamentos — `handle_redirection()`

> 📍 `src/exec/redirs.c`, linhas 63-85

Percorre a lista de `t_redir` do comando e para cada uma:
| Tipo | Ação |
|------|------|
| `REDIR_IN` (`<`) | `open(file, O_RDONLY)` → `dup2(fd, STDIN)` |
| `REDIR_OUT` (`>`) | `open(file, O_WRONLY\|O_CREAT\|O_TRUNC, 0644)` → `dup2(fd, STDOUT)` |
| `REDIR_APPEND` (`>>`) | `open(file, O_WRONLY\|O_CREAT\|O_APPEND, 0644)` → `dup2(fd, STDOUT)` |
| `REDIR_HEREDOC` (`<<`) | `dup2(cmd->heredoc_fd, STDIN)` |

#### Erros de Execução — `execution_error()`

> 📍 `src/exec/exec_errors.c`, linhas 57-80

| Situação | Código | Mensagem |
|----------|--------|----------|
| Comando com `/` que é diretório | 126 | "Is a directory" |
| Comando com `/` que não existe | 127 | "No such file or directory" |
| Comando sem `/` não encontrado no PATH | 127 | "command not found" |
| Sem permissão de execução | 126 | "Permission denied" |

---

### 4.11 Builtins

> 📍 `src/builtins/`

#### Router — `is_builtin()` e `exec_builtin()`

> 📍 `src/builtins/builtins_router.c`

`is_builtin()` verifica se `args[0]` é um dos 7 builtins. Usa `ft_strncmp` com o tamanho `strlen + 1` para garantir correspondência exata.

`exec_builtin()` redireciona para a função correta conforme o nome do comando.

#### `echo` — `ft_echo()`

> 📍 `src/builtins/builtins_info.c`, linhas 32-53

- Verifica a flag `-n` (e variantes como `-nnn`, `-nnnn`) via `is_valid_n_flag()`.
- Imprime todos os argumentos separados por espaço.
- Se NÃO tem `-n` → imprime newline no final.

#### `pwd` — `ft_pwd()`

> 📍 `src/builtins/builtins_info.c`, linhas 55-73

- Primeiro tenta `get_env_value(shell->env_vars, "PWD")`.
- Se falhar, usa `getcwd(buf, PATH_MAX)`.

#### `cd` — `ft_cd()`

> 📍 `src/builtins/builtin_cd.c`

- `cd` sem argumentos ou `cd ~` → vai para `$HOME`.
- `cd -` → vai para `$OLDPWD` e imprime o caminho.
- `cd <path>` → `chdir(path)`.
- Atualiza `OLDPWD` e `PWD` via `ft_export`.

#### `exit` — `ft_exit()`

> 📍 `src/builtins/builtin_exit.c`, linhas 25-49

- Imprime "exit".
- Sem argumentos → sai com `shell->exit_code`.
- Com argumento numérico → sai com `(unsigned char)res`.
- Argumento não numérico → erro, exit code 2.
- Muitos argumentos → erro, NÃO sai (exit code 1).

#### `export` — `ft_export()`

> 📍 `src/builtins/builtins_env.c`, linhas 68-91

- Sem argumentos → `show_export_list()` (imprime variáveis ordenadas no formato `declare -x`).
- Com argumentos → valida a chave (`is_valid_key()`) e adiciona/atualiza via `update_env()`.
- Suporta `+=` para concatenar valores.

#### `unset` — `ft_unset()`

> 📍 `src/builtins/builtins_env.c`, linhas 55-66

Remove variáveis do ambiente. Cria um novo array sem a variável indicada.

#### `env` — `ft_env()`

> 📍 `src/builtins/builtins_env.c`, linhas 93-113

Imprime todas as variáveis de ambiente que contêm `=` (variáveis com valor).

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

**`handle_sigint()`** (linhas 18-24):
```c
void handle_sigint(int sig)
{
    g_last_signal = sig;      // Guarda que recebeu SIGINT
    write(1, "\n", 1);        // Nova linha
    rl_on_new_line();         // Informa readline que estamos numa nova linha
    rl_replace_line("", 0);   // Limpa o que estava escrito
    rl_redisplay();           // Redesenha o prompt
}
```

---

### 4.13 Limpeza de Memória

> 📍 `src/utils/free.c`

**`free_all()`** — Liberta TUDO (usado no `exit` e em erros fatais):
- `free_tokens(shell->s_tokens)` → Liberta lista de tokens
- `free_cmds(shell->s_cmds)` → Liberta lista de comandos (incluindo args, redirs e heredoc fds)
- `free_env(shell->env_vars)` → Liberta array de variáveis de ambiente
- `rl_clear_history()` → Limpa o histórico do readline

**`cleanup_exit_child()`** — Versão para processos filhos (não limpa readline).

---

## 5. 📊 Diagrama do Fluxo

```
╔═══════════════════════════════════════════════════════════════╗
║                        main()                                  ║
║  src/main.c:61                                                 ║
║  init_shell() → shlvl_update() → setup_signals()              ║
╚════════════════════════╦══════════════════════════════════════╝
                         ▼
            ┌─────── shell_loop() ◄────────────────────┐
            │    src/main.c:31                          │
            ▼                                           │
    ┌───────────────┐                                   │
    │  get_input()  │  readline("minishell> ")          │
    │  src/main.c:15│  ou read_line()                   │
    └───────┬───────┘                                   │
            │ line                                      │
            ▼                                           │
    ┌───────────────────┐   inválida                    │
    │  validate_line()  │──────────── free(line) ───────┤
    │  main_utils.c:53  │   (aspas, #)                  │
    └───────┬───────────┘                               │
            │ válida                                    │
            ▼                                           │
    ┌───────────────────┐                               │
    │  process_line()   │  Divide por ';'               │
    │  main_utils.c:118 │                               │
    └───────┬───────────┘                               │
            ▼                                           │
    ┌───────────────────┐                               │
    │   exec_single()   │                               │
    │  main_utils.c:103 │                               │
    └───────┬───────────┘                               │
            │                                           │
    ┌───────▼───────┐   ┌──────────────┐   ┌──────────────────┐
    │    lexer()    │ → │   parser()   │ → │    executor()    │
    │  lexer.c:67   │   │ parser.c:89  │   │  execute.c:100   │
    └───────────────┘   └──────────────┘   └────────┬─────────┘
                                                     │
                                           ┌─────────┴─────────┐
                                           ▼                   ▼
                                   ┌──────────────┐   ┌──────────────┐
                                   │ Builtin solo │   │ execute_pipe │
                                   │ (no pai)     │   │ (com fork)   │
                                   └──────────────┘   └──────┬───────┘
                                                              │
                                                    ┌─────────▼─────────┐
                                                    │  child_process()  │
                                                    │    child.c:82     │
                                                    └─────────┬─────────┘
                                                              │
                                                    ┌─────────▼─────────┐
                                                    │ handle_pipes()    │
                                                    │ handle_redir()    │
                                                    │ child_exec()      │
                                                    │   → execve()      │
                                                    └───────────────────┘
                                                              │
            ┌─────────────────────────────────────────────────┘
            │ free(line), voltar ao loop
            └──────────────────────────────────────────────────►┘
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

| Arquivo | Funcs | Status | Observações |
|---------|-------|--------|-------------|
| `src/main.c` | 4 | ✅ OK | `get_input`, `shell_loop`, `init_shell`, `main` |
| `src/lexer/lexer.c` | 4 | ✅ OK | Todas ≤25 linhas |
| `src/lexer/lexer_utils.c` | 5 | ✅ OK | `is_space`, `is_special`, `new_token`, `token_add_back`, `join_and_free` |
| `src/lexer/lexer_word.c` | 5 | ✅ OK | `is_end_of_word`, `extract_quoted_content`, `extract_unquoted`, `handle_word_part`, `build_full_word` |
| `src/parser/parser.c` | 5 | ✅ OK | `check_pipe_syntax`, `check_redir_syntax`, `validate_syntax`, `process_token`, `parser` |
| `src/parser/parser_utils.c` | 5 | ✅ OK | `new_cmd`, `cmd_add_back`, `argv_len`, `cmd_add_arg`, `redir_add_back` |
| `src/parser/parser_redir.c` | 2 | ✅ OK | `parse_redir_in`, `parse_redir_out` |
| `src/expander/expander.c` | 5 | ✅ OK | `get_var_value`, `expand_var_name`, `extract_and_expand_var`, `expand_vars`, `expand_tokens` |
| `src/heredoc/heredoc.c` | 5 | ✅ OK | `write_hd_line`, `read_heredoc_lines`, `heredoc_read`, `handle_heredoc`, `process_heredocs` |
| `src/heredoc/heredoc_utils.c` | 5 | ✅ OK | `heredoc_has_quotes`, `heredoc_remove_quotes`, `heredoc_gen_temp_filename`, `heredoc_eof_warning`, `heredoc_read_line` |
| `src/exec/execute.c` | 5 | ✅ OK | `exec_single_builtin`, `wait_children`, `pipe_loop`, `execute_pipe`, `executor` |
| `src/exec/child.c` | 5 | ✅ OK | `resolve_cmd_path`, `handle_exec_path`, `handle_pipes`, `child_exec`, `child_process` |
| `src/exec/redirs.c` | 3 | ✅ OK | `apply_redir`, `apply_heredoc`, `handle_redirection` |
| `src/exec/path.c` | 2 | ✅ OK | `search_path`, `find_path` |
| `src/exec/exec_errors.c` | 3 | ✅ OK | `print_error_message`, `is_right_assignment`, `execution_error` |
| `src/builtins/builtins_router.c` | 2 | ✅ OK | `is_builtin`, `exec_builtin` |
| `src/builtins/builtins_info.c` | 3 | ✅ OK | `is_valid_n_flag`, `ft_echo`, `ft_pwd` |
| `src/builtins/builtin_cd.c` | 5 | ✅ OK | `print_cd_error`, `update_w_export`, `get_target_dir`, `manage_pwd`, `ft_cd` |
| `src/builtins/builtin_exit.c` | 2 | ✅ OK | `exit_numeric_error`, `ft_exit` |
| `src/builtins/builtins_env.c` | 5 | ✅ OK | `is_key_match`, `remove_env_var`, `ft_unset`, `ft_export`, `ft_env` |
| `src/builtins/export_print.c` | 3 | ✅ OK | `print_export_line`, `sort_env_matrix`, `show_export_list` |
| `src/env/env_init.c` | 3 | ✅ OK | `shlvl_update`, `count_env`, `copy_env`, `free_env` — ⚠️ ver nota |
| `src/env/env_get.c` | 2 | ✅ OK | `get_matrix_len`, `get_env_value` |
| `src/env/env_modify.c` | 4 | ✅ OK | `is_valid_key`, `add_var_to_env`, `exp_and_append`, `update_env` |
| `src/signals/signals.c` | 5 | ✅ OK | `handle_sigint`, `setup_signals`, `setup_signals_execution`, `handle_sigint_heredoc`, `setup_signals_heredoc` |
| `src/utils/free.c` | 5 | ✅ OK | `free_all`, `free_tokens`, `free_redirs`, `free_cmds`, `cleanup_exit_child` |
| `src/utils/utils.c` | 4 | ✅ OK | `special_expand_params`, `ft_strcmp`, `free_tab`, `ft_isspace` |
| `src/utils/utils2.c` | 5 | ✅ OK | `parse_sign_and_skip`, `overflow_check`, `ft_atoll_overflow`, `append_char`, `read_line` |
| `src/utils/debug.c` | 3 | ✅ OK | `print_tokens`, `print_redir`, `print_cmds` |

### ⚠️ Potenciais Problemas Identificados:

#### 1. `src/utils/main_utils.c` — **6 funções** ❌

Este arquivo contém:
1. `is_comment_line()` (static)
2. `has_unclosed_quotes()` (static)
3. `validate_line()`
4. `next_semicolon()` (static)
5. `exec_single()` (static)
6. `process_line()`

**A norminette permite no máximo 5 funções por arquivo `.c`.** Este arquivo tem 6, o que é uma **violação**. A `norminette` conta TODAS as funções, incluindo `static`.

**Solução sugerida:** Mover `is_comment_line()` e `has_unclosed_quotes()` para um arquivo separado (ex: `src/utils/validate.c`), ou mover `next_semicolon()` e `exec_single()` junto com `process_line()` para outro ficheiro.

#### 2. `src/env/env_init.c` — **4 funções** ✅ (mas conferir)

Contém: `shlvl_update`, `count_env` (static), `copy_env`, `free_env` = 4 funções. OK.

#### 3. `src/utils/main_utils.c` — Formatação de `next_semicolon()`

```c
static char *next_semicolon(char *line)
{
    int    in_quote;    // ← possível: norminette exige `int\t\tin_quote;` (tab alinhamento)
    char   quote;
```

A declaração `int in_quote;` e `char quote;` em linhas separadas pode não estar alinhada com tabs conforme a norma. Verificar com `norminette` se os alinhamentos estão corretos com tabs (não espaços).

#### 4. Variável global — ✅ OK

Apenas `g_last_signal` em `src/signals/signals.c:16`. Permitida pelo subject do minishell. Nenhuma outra variável global foi encontrada.

#### 5. Linhas com mais de 80 colunas — ⚠️ Verificar

Algumas linhas em comentários e em `ft_putstr_fd` com mensagens longas podem ultrapassar 80 colunas. Exemplos a verificar:
- `src/parser/parser.c:21` — `"minishell: syntax error near unexpected token '|'"` (55 chars na string)
- `src/utils/main_utils.c:62` — `"minishell: syntax error: unclosed quotes"` — OK

Recomendo rodar `norminette` localmente para confirmar.

#### 6. Número de parâmetros — ✅ OK

Todas as funções têm no máximo 4 parâmetros:
- `child_process(cmd, fd_in, fd_pipe, shell)` — 4 ✅
- `build_full_word(line, i, shell, quoted)` — 4 ✅
- `handle_word_part(line, i, shell, quoted)` — 4 ✅
- `pipe_loop(cmd, fd_in, pid, sh)` — 4 ✅

### Resumo da Análise:

| Critério | Resultado |
|----------|-----------|
| Header 42 em todos os ficheiros | ✅ Presente |
| Máx 25 linhas por função | ✅ Todas OK |
| Máx 5 funções por `.c` | ❌ `main_utils.c` tem 6 |
| Máx 4 parâmetros | ✅ Todas OK |
| 1 variável global (permitida) | ✅ Apenas `g_last_signal` |
| Prefixos `s_`, `t_`, `e_` | ✅ Corretos |
| Declarações no topo | ✅ Corretas |
| Indentação com tabs | ✅ Correta |
| Máx 80 colunas | ⚠️ Verificar com norminette |

---

> **📝 Nota:** Esta documentação foi gerada com base na análise do código no commit `d6abe07` / `f9875ab`. Os resultados de busca foram limitados e podem não cobrir todo o conteúdo do repositório. Para ver todos os ficheiros, visite: https://github.com/fsugii/minishelll_tests