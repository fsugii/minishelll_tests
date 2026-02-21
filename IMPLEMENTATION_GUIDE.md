# Minishell - Complete Implementation Guide

## Table of Contents

1. [Introduction](#introduction)
2. [Data Structures](#data-structures)
3. [The Pipeline](#the-pipeline)
4. [Main Loop](#main-loop)
5. [Lexer (Tokenization)](#lexer-tokenization)
6. [Expander (Variable Expansion)](#expander-variable-expansion)
7. [Parser](#parser)
8. [Executor](#executor)
9. [Builtins](#builtins)
10. [Heredoc](#heredoc)
11. [Signals](#signals)
12. [Environment Variables](#environment-variables)

---

## Introduction

Minishell is a simplified version of the Unix shell (like bash). It reads user input, processes it, and executes commands. This implementation follows this architecture:

```
User Input → Lexer → Expander → Parser → Executor → Command Output
```

Each component transforms the data before passing it to the next stage.

---

## Data Structures

### t_token_type (Enum)

Defines the types of tokens the lexer can produce:

```c
typedef enum e_token_type
{
    TK_WORD,       // Regular words/arguments (e.g., "ls", "-la", "file.txt")
    TK_PIPE,       // Pipe character: |
    TK_REDIR_IN,   // Input redirection: <
    TK_REDIR_OUT,  // Output redirection: >
    TK_APPEND,     // Append redirection: >>
    TK_HEREDOC,    // Heredoc: <<
} t_token_type;
```

### t_token

Represents a single token after lexing:

```c
typedef struct s_token
{
    t_token_type  type;       // What kind of token is this?
    char          *value;     // The actual text content
    int           no_expand;  // Should we expand $ variables? (for single quotes)
    int           in_dquotes; // Did this come from double quotes?
    struct s_token *next;    // Pointer to next token (linked list)
} t_token;
```

### t_redir_type (Enum)

```c
typedef enum e_redir_type
{
    REDIR_IN,      // <
    REDIR_OUT,    // >
    REDIR_APPEND, // >>
    REDIR_HEREDOC,// <<
} t_redir_type;
```

### t_redir

Represents a redirection:

```c
typedef struct s_redir
{
    t_redir_type  type;       // Type of redirection
    char          *target;    // Filename or heredoc delimiter
    struct s_redir *next;    // Next redirection (linked list)
} t_redir;
```

### t_cmd

Represents a command in a pipeline:

```c
typedef struct s_cmd
{
    char          **args;     // Command arguments (e.g., ["ls", "-la", NULL])
    t_redir       *redirs;   // List of redirections
    char          **limits;   // Heredoc delimiters
    int           heredoc_fd; // File descriptor for heredoc input
    struct s_cmd  *next;     // Next command in pipeline (for pipes)
} t_cmd;
```

### t_shell

Global shell state:

```c
typedef struct s_shell
{
    char          **env_vars;     // Environment variables (e.g., ["PATH=/bin", "HOME=/home/user"])
    char          **export_vars;  // Exported variables
    int           exit_code;      // Last command's exit code (0-255)
    t_token       *s_tokens;      // Current tokens (for cleanup)
    t_cmd         *s_cmds;        // Current commands (for cleanup)
} t_shell;
```

---

## The Pipeline

Here's how data flows through the shell:

```
┌─────────────────────────────────────────────────────────────────┐
│                         USER INPUT                              │
│                    "ls -la | grep .c > out"                     │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                            LEXER                                 │
│  Tokenizes input into a linked list of tokens                   │
│  [TK_WORD:"ls"] [TK_WORD:"-la"] [TK_PIPE:"|"] [TK_WORD:"grep"] │
│  [TK_WORD:".c"] [TK_REDIR_OUT:">"] [TK_WORD:"out"]             │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                          EXPANDER                               │
│  Expands $VARIABLES, handles quotes                            │
│  $HOME → /home/user, "hello $USER" → "hello fshiniti"          │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                           PARSER                                │
│  Builds command list from tokens                               │
│  Creates t_cmd structures with args and redirections           │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                         EXECUTOR                                │
│  Forks processes, handles pipes, executes commands             │
│  Manages redirections, waits for children                      │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                         BUILTINS                                │
│  Built-in commands like cd, echo, export, etc.                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## Main Loop

The entry point and main control flow:

### main.c

```c
int main(int argc, char **argv, char **envp)
{
    t_shell shell;
    
    // Initialize shell with environment variables
    shell.env_vars = copy_env(envp);
    shell.exit_code = 0;
    shell.s_tokens = NULL;
    shell.s_cmds = NULL;
    
    // Update shell level (for nested shells)
    shlvl_update(&shell.env_vars);
    
    // Setup signal handlers (Ctrl+C, Ctrl+\)
    setup_signals();
    
    // Enter main loop
    shell_loop(&shell);
    
    return 0;
}
```

### shell_loop()

```c
static void shell_loop(t_shell *shell)
{
    char *line;
    
    while (1)
    {
        // Get input from user
        line = get_input(shell);
        
        // Validate input (comments, unclosed quotes)
        if (validate_line(line, shell))
            continue;  // Skip to next iteration
        
        // Process the line: lexer → parser → executor
        process_line(line, shell);
        
        // Loop continues...
    }
}
```

### get_input()

Gets user input, supporting both interactive and non-interactive modes:

```c
static char *get_input(t_shell *shell)
{
    char *line;
    
    // Check if running in terminal or piped
    if (isatty(STDIN_FILENO))
        line = readline("minishell> ");  // Interactive: show prompt
    else
        line = read_line();  // Non-interactive: read from pipe
    
    // Handle Ctrl+C (SIGINT)
    if (g_last_signal == SIGINT)
    {
        shell->exit_code = 130;  // Standard bash exit code for Ctrl+C
        g_last_signal = 0;
    }
    
    return line;
}
```

### validate_line()

Validates the input before processing:

```c
int validate_line(char *line, t_shell *shell)
{
    // Check if it's a comment line (starts with #)
    if (is_comment_line(line))
    {
        shell->exit_code = 0;
        free(line);
        return 1;  // Skip processing
    }
    
    // Check for unclosed quotes
    if (has_unclosed_quotes(line))
    {
        ft_putendl_fd("minishell: syntax error: unclosed quotes", 2);
        shell->exit_code = 2;  // Syntax error
        free(line);
        return 1;  // Skip processing
    }
    
    return 0;  // Continue processing
}
```

### process_line()

The core processing function:

```c
void process_line(char *line, t_shell *shell)
{
    t_token *tokens;
    t_cmd   *cmds;
    
    // Step 1: LEXER - Convert string to tokens
    tokens = lexer(line, shell);
    
    if (tokens)
    {
        shell->s_tokens = tokens;  // Save for cleanup
        
        // Step 2: PARSER - Convert tokens to commands
        cmds = parser(tokens, shell);
        
        if (cmds)
        {
            shell->s_cmds = cmds;
            
            // Step 3: EXECUTOR - Run the commands
            executor(cmds, shell);
            
            // Cleanup commands
            free_cmds(cmds);
            shell->s_cmds = NULL;
        }
        
        // Cleanup tokens
        free_tokens(tokens);
        shell->s_tokens = NULL;
    }
}
```

---

## Lexer (Tokenization)

The lexer converts the raw input string into a linked list of tokens.

### lexer()

Main lexing function:

```c
t_token *lexer(char *line, t_shell *shell)
{
    t_token *tokens = NULL;
    int    i = 0;
    
    if (!line)
        return NULL;
    
    // Process each character
    while (line[i])
    {
        process_char(&tokens, line, &i, shell);
        i++;
    }
    
    return tokens;
}
```

### process_char()

Processes a single character and creates appropriate tokens:

```c
static void process_char(t_token **tokens, char *line, int *i, t_shell *shell)
{
    char *word;
    t_token *tok;
    int  quoted;
    
    // Skip whitespace
    if (is_space(line[*i]))
        return ;
    
    // Pipe character
    else if (line[*i] == '|')
        token_add_back(tokens, new_token("|", TK_PIPE));
    
    // Input redirection (< or <<)
    else if (line[*i] == '<')
        handle_redir_in(tokens, line, i);
    
    // Output redirection (> or >>)
    else if (line[*i] == '>')
        handle_redir_out(tokens, line, i);
    
    // Regular word (command, argument, filename)
    else
    {
        // Build the complete word (handles quotes)
        word = build_full_word(line, i, shell, &quoted);
        
        // Create token
        tok = new_token(word, TK_WORD);
        
        // Mark if it came from quotes
        if (tok && quoted)
            tok->in_dquotes = 1;
        
        token_add_back(tokens, tok);
        free(word);
        (*i)--;  // Adjust index since we already advanced
    }
}
```

### build_full_word()

Builds a complete word, handling quoted sections:

```c
char *build_full_word(char *line, int *i, t_shell *shell, int *quoted)
{
    char *result;
    char *part;
    
    result = ft_strdup("");  // Start with empty string
    *quoted = 0;
    
    // Continue until end of word (space, special char, or null)
    while (line[*i] && !is_end_of_word(line[*i]))
    {
        // Handle each part of the word
        part = handle_word_part(line, i, shell, quoted);
        result = join_and_free(result, part);
    }
    
    return result;
}
```

### handle_word_part()

Handles a single part of a word (quoted or unquoted):

```c
static char *handle_word_part(char *line, int *i, t_shell *shell, int *quoted)
{
    char *part;
    char *expanded;
    
    // Single quotes: no expansion
    if (line[*i] == '\'')
    {
        *quoted = 1;
        return extract_quoted_content(line, i);  // Return content literally
    }
    
    // Double quotes: expand $variables
    else if (line[*i] == '"')
    {
        *quoted = 1;
        part = extract_quoted_content(line, i);
        
        // Expand variables within double quotes
        expanded = expand_vars(part, shell->env_vars, shell->exit_code);
        free(part);
        return expanded;
    }
    
    // No quotes: extract and expand
    part = extract_unquoted(line, i);
    expanded = expand_vars(part, shell->env_vars, shell->exit_code);
    free(part);
    return expanded;
}
```

### Example

Input: `echo "$HOME world"`

```
1. 'e' - not special → build word
2. In build_word:
   - See '"' → handle as double quote
   - Extract "HOME" inside quotes
   - Expand $HOME → "/home/fshiniti"
   - Continue to " world" (unquoted)
   - Expand nothing
3. Result: "fshiniti world"
```

---

## Expander (Variable Expansion)

The expander replaces `$VARIABLE` with their values.

### expand_vars()

Main expansion function:

```c
char *expand_vars(char *str, char **env, int last_exit)
{
    char *result;
    char *expanded;
    
    if (!str || !*str)
        return ft_strdup(str ? str : "");
    
    result = ft_strdup("");
    
    // Process each character
    while (*str)
    {
        if (*str == '$')
        {
            // Handle special parameters ($?, $$)
            if (*(str + 1) == '?')
            {
                expanded = ft_itoa(last_exit);  // Exit code
                str += 2;
            }
            else if (*(str + 1) == '$')
            {
                expanded = ft_itoa(getpid());  // Process ID
                str += 2;
            }
            // Handle $VAR or ${VAR}
            else
            {
                expanded = extract_and_expand_var(str, &str, env);
            }
            
            // Add expanded value
            result = join_and_free(result, expanded);
        }
        else
        {
            // Regular character - add as is
            result = join_and_free(result, char_to_str(*str));
            str++;
        }
    }
    
    return result;
}
```

### extract_and_expand_var()

Extracts variable name and looks up its value:

```c
static char *extract_and_expand_var(char *str, char **end, char **env)
{
    char *var_name;
    char *value;
    
    str++;  // Skip $
    
    // Handle ${VAR} syntax
    if (*str == '{')
    {
        str++;
        var_name = extract_var_name(str, '}');
        *end = str + ft_strlen(var_name) + 2;  // Skip to after }
    }
    // Handle $VAR syntax
    else
    {
        var_name = extract_var_name(str, '\0');
        *end = str + ft_strlen(var_name);
    }
    
    // Look up in environment
    value = get_env_value(env, var_name);
    free(var_name);
    
    return ft_strdup(value ? value : "");
}
```

### Example

```
Input: "Hello $USER, your home is $HOME"
Environment: USER=fshiniti, HOME=/home/fshiniti

Output: "Hello fshiniti, your home is /home/fshiniti"
```

---

## Parser

The parser converts tokens into a structured command list.

### parser()

Main parsing function:

```c
t_cmd *parser(t_token *tokens, t_shell *shell)
{
    t_cmd *cmds;
    t_cmd *current_cmd;
    
    if (!tokens)
        return NULL;
    
    // Validate syntax first
    if (validate_syntax(tokens) != 0)
    {
        shell->exit_code = 2;
        return NULL;
    }
    
    // Initialize command list
    cmds = NULL;
    current_cmd = new_cmd();
    cmd_add_back(&cmds, current_cmd);
    
    // Process each token
    while (tokens)
    {
        process_token(&cmds, &current_cmd, &tokens);
        tokens = tokens->next;
    }
    
    return cmds;
}
```

### process_token()

Processes a single token:

```c
static void process_token(t_cmd **cmds, t_cmd **cur, t_token **tokens)
{
    // Word token - add as argument
    if ((*tokens)->type == TK_WORD)
    {
        // Skip empty words unless from quotes or no_expand
        if ((*tokens)->value[0] != '\0' || (*tokens)->no_expand
            || (*tokens)->in_dquotes)
            cmd_add_arg(*cur, (*tokens)->value);
    }
    
    // Pipe token - create new command
    else if ((*tokens)->type == TK_PIPE)
    {
        *cur = new_cmd();
        cmd_add_back(cmds, *cur);
    }
    
    // Input redirection (< or <<)
    else if ((*tokens)->type == TK_REDIR_IN
        || (*tokens)->type == TK_HEREDOC)
        parse_redir_in(*cur, tokens);
    
    // Output redirection (> or >>)
    else if ((*tokens)->type == TK_REDIR_OUT
        || (*tokens)->type == TK_APPEND)
        parse_redir_out(*cur, tokens);
}
```

### parse_redir_in()

Parses input redirection:

```c
void parse_redir_in(t_cmd *current_cmd, t_token **tokens)
{
    t_redir *redir;
    
    redir = malloc(sizeof(t_redir));
    if (!redir)
        return ;
    
    // Determine type: < or <<
    if ((*tokens)->type == TK_HEREDOC)
        redir->type = REDIR_HEREDOC;
    else
        redir->type = REDIR_IN;
    
    // Move to next token (the target)
    *tokens = (*tokens)->next;
    
    // Get the target filename or delimiter
    if (*tokens && (*tokens)->type == TK_WORD)
        redir->target = ft_strdup((*tokens)->value);
    else
        redir->target = NULL;
    
    redir->next = NULL;
    redir_add_back(&current_cmd->redirs, redir);
}
```

### Example

Tokens: `[WORD:"ls"] [WORD:"-la"] [PIPE:"|"] [WORD:"grep"] [WORD:"main"]`

```
Parser builds:
cmd1: args=["ls", "-la"], redirs=NULL
cmd2: args=["grep", "main"], redirs=NULL
```

With pipes: `[WORD:"ls"] [REDIR_OUT:">"] [WORD:"out"]`

```
cmd1: args=["ls"], redirs=[REDIR_OUT:"out"]
```

---

## Executor

The executor runs commands, handling pipes, redirections, and forking processes.

### executor()

Main execution dispatcher:

```c
void executor(t_cmd *cmd, t_shell *shell)
{
    if (!cmd)
        return ;
    
    // Process heredocs first (before forking)
    process_heredocs(cmd, shell->env_vars, shell->exit_code);
    
    // Handle variable assignment (e.g., VAR=value)
    if (!cmd->next && cmd->args && is_right_assignment(cmd->args[0]))
    {
        update_env(cmd->args[0], &shell->env_vars);
        shell->exit_code = 0;
        return ;
    }
    
    // Single builtin - execute in parent process
    else if (!cmd->next && cmd->args && is_builtin(cmd->args))
        exec_single_builtin(cmd, shell);
    
    // Pipeline or external command - use fork
    else
        execute_pipe(cmd, shell);
}
```

### execute_pipe()

Handles pipeline execution (multiple commands with pipes):

```c
void execute_pipe(t_cmd *cmd, t_shell *shell)
{
    int  fd_pipe[2];
    int  fd_in;
    pid_t pid;
    
    fd_in = -1;  // No initial input
    
    // Setup signals for execution (ignore Ctrl+C in child)
    setup_signals_execution();
    
    // Process each command in the pipeline
    while (cmd)
    {
        // Create pipe to next command (if there is one)
        if (cmd->next && pipe(fd_pipe) == -1)
            return ;
        
        // Fork child process
        pid = fork();
        
        if (pid == 0)
            // Child: execute the command
            child_process(cmd, fd_in, fd_pipe, shell);
        
        // Parent: cleanup and prepare for next command
        if (fd_in != -1)
            close(fd_in);
        
        if (cmd->next)
        {
            close(fd_pipe[1]);           // Close write end
            fd_in = fd_pipe[0];          // Read from pipe for next cmd
        }
        
        cmd = cmd->next;
    }
    
    // Wait for all children and get exit code
    wait_children(pid, shell);
    
    // Restore signals for interactive mode
    setup_signals();
}
```

### child_process()

Executes a single command in a child process:

```c
void child_process(t_cmd *cmd, int fd_in, int *fd_pipe, t_shell *shell)
{
    // Setup signal handlers for child (default - terminate on signal)
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    
    // Setup pipes: connect input from previous command
    handle_pipes(cmd, fd_in, fd_pipe);
    
    // Handle redirections (>, <, >>, <<)
    if (handle_redirection(cmd) != 0)
        cleanup_exit_child(shell, 1);
    
    // Check for builtins
    if (is_builtin(cmd->args))
    {
        int status = exec_builtin(cmd, shell);
        cleanup_exit_child(shell, status);
    }
    
    // External command - use execve
    handle_exec_path(cmd, shell);
}
```

### handle_pipes()

Connects pipes between commands:

```c
void handle_pipes(t_cmd *cmd, int fd_in, int *fd_pipe)
{
    // Connect input from previous command
    if (fd_in != -1)
    {
        dup2(fd_in, STDIN_FILENO);  // Redirect stdin
        close(fd_in);               // Close original
    }
    
    // Connect output to next command
    if (cmd->next)
    {
        close(fd_pipe[0]);                  // Close read end
        dup2(fd_pipe[1], STDOUT_FILENO);  // Redirect stdout
        close(fd_pipe[1]);                  // Close original
    }
}
```

### handle_redirection()

Processes file redirections:

```c
int handle_redirection(t_cmd *cmd)
{
    t_redir *redir;
    int     fd;
    
    if (!cmd->redirs)
        return 0;
    
    redir = cmd->redirs;
    
    while (redir)
    {
        // Handle each redirection type
        if (redir->type == REDIR_IN)
        {
            fd = open(redir->target, O_RDONLY);
            check_open(redir->target, O_RDONLY, STDIN_FILENO);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        else if (redir->type == REDIR_OUT)
        {
            fd = open(redir->target, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            check_open(redir->target, O_WRONLY, STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (redir->type == REDIR_APPEND)
        {
            fd = open(redir->target, O_CREAT | O_WRONLY | O_APPEND, 0644);
            check_open(redir->target, O_WRONLY, STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (redir->type == REDIR_HEREDOC)
        {
            // Heredoc already processed, use the fd
            dup2(cmd->heredoc_fd, STDIN_FILENO);
            close(cmd->heredoc_fd);
        }
        
        redir = redir->next;
    }
    
    return 0;
}
```

---

## Builtins

Built-in commands that run in the shell process itself.

### is_builtin()

Checks if a command is a built-in:

```c
int is_builtin(char **args)
{
    if (!args || !args[0])
        return 0;
    
    return (strcmp(args[0], "echo") == 0 ||
            strcmp(args[0], "cd") == 0 ||
            strcmp(args[0], "pwd") == 0 ||
            strcmp(args[0], "export") == 0 ||
            strcmp(args[0], "unset") == 0 ||
            strcmp(args[0], "env") == 0 ||
            strcmp(args[0], "exit") == 0);
}
```

### exec_builtin()

Routes to the appropriate built-in:

```c
int exec_builtin(t_cmd *cmd, t_shell *shell)
{
    char **args = cmd->args;
    
    if (strcmp(args[0], "echo") == 0)
        return ft_echo(args);
    else if (strcmp(args[0], "cd") == 0)
        return ft_cd(args, &shell->env_vars);
    else if (strcmp(args[0], "pwd") == 0)
        return ft_pwd(shell);
    else if (strcmp(args[0], "export") == 0)
        return ft_export(args, &shell->env_vars);
    else if (strcmp(args[0], "unset") == 0)
        return ft_unset(args, &shell->env_vars);
    else if (strcmp(args[0], "env") == 0)
        return ft_env(args, shell->env_vars);
    else if (strcmp(args[0], "exit") == 0)
        return ft_exit(args, shell);
    
    return 0;
}
```

### Builtin Examples

#### echo

```c
int ft_echo(char **args)
{
    int n_flag = 0;
    int i;
    
    // Check for -n flag
    if (args[1] && strcmp(args[1], "-n") == 0)
        n_flag = 1;
    
    // Print each argument
    i = 1 + n_flag;
    while (args[i])
    {
        ft_putstr_fd(args[i], STDOUT_FILENO);
        if (args[i + 1])
            ft_putchar_fd(' ', STDOUT_FILENO);
        i++;
    }
    
    // Newline unless -n
    if (!n_flag)
        ft_putchar_fd('\n', STDOUT_FILENO);
    
    return 0;
}
```

#### cd

```c
int ft_cd(char **args, char ***env)
{
    char *path;
    
    // cd with no args = cd ~
    if (!args[1])
        path = get_env_value(*env, "HOME");
    // cd - = cd to previous directory
    else if (strcmp(args[1], "-") == 0)
        path = get_env_value(*env, "OLDPWD");
    // cd <path>
    else
        path = args[1];
    
    // Change directory
    if (chdir(path) != 0)
    {
        ft_putstr_fd("minishell: cd: ", 2);
        ft_putstr_fd(path, 2);
        ft_putstr_fd(": No such file or directory\n", 2);
        return 1;
    }
    
    // Update PWD and OLDPWD
    update_pwd(env);
    
    return 0;
}
```

---

## Heredoc

Heredoc (`<<`) allows multi-line input redirection.

### How it works

```
Input: << EOF
       line 1
       line 2
       EOF
```

The shell reads lines until it sees the delimiter, then passes them as input.

### handle_heredoc()

Main heredoc processing:

```c
int handle_heredoc(char *delimiter, char **env, int last_exit)
{
    t_hd_ctx ctx;
    char *clean_delimiter;
    char *temp_file;
    int  fd;
    
    // Check if we should expand variables (no quotes = expand)
    ctx.expand = !heredoc_has_quotes(delimiter);
    ctx.env = env;
    ctx.exit_code = last_exit;
    
    // Remove quotes from delimiter if present
    clean_delimiter = heredoc_remove_quotes(delimiter);
    
    // Create temporary file
    temp_file = heredoc_gen_temp_filename();
    ctx.fd = open(temp_file, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    
    if (ctx.fd < 0)
    {
        perror("minishell: heredoc");
        free(clean_delimiter);
        free(temp_file);
        return -1;
    }
    
    // Read lines and write to file
    read_heredoc_lines(&ctx, clean_delimiter);
    
    free(clean_delimiter);
    close(ctx.fd);
    
    // Reopen as read-only and delete temp file
    fd = open(temp_file, O_RDONLY);
    unlink(temp_file);
    free(temp_file);
    
    return fd;
}
```

### read_heredoc_lines()

Reads lines from user:

```c
static void read_heredoc_lines(t_hd_ctx *ctx, char *delimiter)
{
    char *line;
    
    while (1)
    {
        // Get line (supports non-tty mode)
        if (isatty(STDIN_FILENO))
            line = readline("> ");
        else
            line = heredoc_read_line();
        
        // Check for EOF (Ctrl+D)
        if (!line)
        {
            ft_putstr_fd("minishell: warning: here-document delimited ", 2);
            ft_putstr_fd("by end-of-file (wanted '", 2);
            ft_putstr_fd(delimiter, 2);
            ft_putstr_fd("')\n", 2);
            break ;
        }
        
        // Check for delimiter (end of heredoc)
        if (ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            break ;
        }
        
        // Expand variables if not quoted
        write_hd_line(ctx, line);
        
        free(line);
    }
}
```

---

## Signals

Signals allow the shell to handle user interrupts.

### Signal Types

| Signal | Default Action | Our Handling |
|--------|---------------|--------------|
| SIGINT (Ctrl+C) | Terminate | Clear line, show new prompt |
| SIGQUIT (Ctrl+\) | Core dump | Print "Quit" message |
| SIGTSTP (Ctrl+Z) | Stop | Not supported in minishell |

### setup_signals()

Configures signals for interactive mode:

```c
void setup_signals(void)
{
    struct sigaction sa;
    
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // Restart interrupted syscalls
    
    sigaction(SIGINT, &sa, NULL);   // Ctrl+C
    signal(SIGQUIT, SIG_IGN);       // Ignore Ctrl+\
}
```

### handle_sigint()

Handles Ctrl+C:

```c
void handle_sigint(int sig)
{
    // Write newline and new prompt
    ft_putchar_fd('\n', STDOUT_FILENO);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    
    // Set global flag for main loop
    g_last_signal = sig;
}
```

---

## Environment Variables

The shell maintains environment variables that are passed to commands.

### copy_env()

Copies environment from parent process:

```c
char **copy_env(char **envp)
{
    char **env;
    int   i;
    
    if (!envp)
        return NULL;
    
    // Count variables
    i = 0;
    while (envp[i])
        i++;
    
    // Allocate and copy
    env = malloc(sizeof(char *) * (i + 1));
    if (!env)
        return NULL;
    
    i = 0;
    while (envp[i])
    {
        env[i] = ft_strdup(envp[i]);
        i++;
    }
    env[i] = NULL;
    
    return env;
}
```

### get_env_value()

Looks up a variable:

```c
char *get_env_value(char **env, char *key)
{
    int key_len;
    int i;
    
    if (!env || !key)
        return NULL;
    
    key_len = ft_strlen(key);
    
    // Search for KEY=
    i = 0;
    while (env[i])
    {
        if (ft_strncmp(env[i], key, key_len) == 0 && env[i][key_len] == '=')
            return env[i] + key_len + 1;  // Return value after =
        i++;
    }
    
    return NULL;
}
```

### ft_export()

Exports a variable:

```c
int ft_export(char **args, char ***env)
{
    char *key;
    int  i;
    
    // export with no args = show all exported vars
    if (!args[1])
    {
        show_export_list(*env);
        return 0;
    }
    
    // Validate key (must start with letter or _)
    if (!is_valid_key(args[1]))
    {
        ft_putstr_fd("minishell: export: `", 2);
        ft_putstr_fd(args[1], 2);
        ft_putstr_fd("': not a valid identifier\n", 2);
        return 1;
    }
    
    // Add or update variable
    i = 0;
    while ((*env)[i])
    {
        key = ft_substr(args[1], 0, ft_strchr(args[1], '=') - args[1]);
        if (ft_strncmp((*env)[i], key, ft_strlen(key)) == 0)
        {
            free((*env)[i]);
            (*env)[i] = ft_strdup(args[1]);
            free(key);
            return 0;
        }
        free(key);
        i++;
    }
    
    // Not found - add new
    add_var_to_env(env, args[1]);
    
    return 0;
}
```

---

## Memory Management

### free_tokens()

Frees the token linked list:

```c
void free_tokens(t_token *list)
{
    t_token *tmp;
    
    while (list)
    {
        tmp = list;
        list = list->next;
        free(tmp->value);
        free(tmp);
    }
}
```

### free_cmds()

Frees the command linked list:

```c
void free_cmds(t_cmd *cmds)
{
    t_cmd *cmd_tmp;
    t_redir *redir_tmp;
    
    while (cmds)
    {
        cmd_tmp = cmds;
        
        // Free arguments
        free_tab(cmds->args);
        
        // Free redirections
        while (cmds->redirs)
        {
            redir_tmp = cmds->redirs;
            cmds->redirs = cmds->redirs->next;
            free(redir_tmp->target);
            free(redir_tmp);
        }
        
        cmds = cmds->next;
        free(cmd_tmp);
    }
}
```

---

## Summary

This minishell implementation demonstrates:

1. **Tokenization**: Converting raw input into structured tokens
2. **Expansion**: Replacing variables with their values
3. **Parsing**: Building command structures from tokens
4. **Execution**: Running commands with proper pipe and redirection handling
5. **Builtins**: Implementing shell commands in the shell itself
6. **Signals**: Handling user interrupts gracefully
7. **Memory**: Proper allocation and cleanup

The architecture is modular, with clear separation between lexing, parsing, and execution phases, making it easier to understand and maintain.

---

*Generated on 2026-02-21*
