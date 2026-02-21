# Minishell: Original vs Refactoredv3

## Overview

This document details all the changes between the original minishell implementation (`/Documents/42_Porto/minishell/`) and the refactored version (`/Desktop/minishell/Refactoredv3/`).

---

## New Files (only in Refactoredv3)

| File | Description |
|------|-------------|
| `src/main_utils.c` | Auxiliary main functions: `validate_line`, `process_line`, `has_unclosed_quotes`, `is_comment_line` |
| `src/parser/parser_redir.c` | Separated redirection parsing functions: `parse_redir_in`, `parse_redir_out` |
| `src/utils/utils2.c` | Utility functions: `ft_atoll_overflow`, `read_line` (for non-tty mode) |
| `src/heredoc/heredoc_utils.c` | Heredoc auxiliary functions: `heredoc_has_quotes`, `heredoc_remove_quotes`, `heredoc_gen_temp_filename`, `heredoc_read_line` |
| `src/lexer/lexer_word.c` | Word building function: `build_full_word` |

---

## Modified Files

### 1. lexer/lexer.c

| Original | Refactoredv3 |
|----------|---------------|
| Functions `is_end_of_word`, `extract_quoted_content`, `expand_and_join`, `extract_unquoted`, `build_full_word` defined inline | Functions moved to `lexer_word.c` |
| `build_full_word(line, i, env, last_exit)` receives `env` and `last_exit` directly | `build_full_word(line, i, shell, &quoted)` receives `t_shell*` and uses `shell->env_vars`, `shell->exit_code` |
| Does not mark tokens as "quoted" | Marks tokens with `tok->in_dquotes = 1` if they come from quotes |
| `<` handling directly in `process_char` | Extracted to `handle_redir_in()` |

---

### 2. parser/parser.c

| Original | Refactoredv3 |
|----------|---------------|
| `handle_redir_in()` and `handle_redir_out()` inline | Functions moved to `parser_redir.c` as `parse_redir_in()` and `parse_redir_out()` |
| Validation in single function `validate_syntax()` | Separated into `check_pipe_syntax()` and `check_redir_syntax()` |

---

### 3. heredoc/heredoc.c

| Original | Refactoredv3 |
|----------|---------------|
| `has_quotes()`, `remove_quotes()`, `generate_temp_filename()` inline | Moved to `heredoc_utils.c` |
| `read_heredoc_lines()` uses only `readline("> ")` | Supports non-tty: `isatty(STDIN_FILENO) ? readline() : heredoc_read_line()` |
| Parameters passed individually | Uses `t_hd_ctx` struct to group `fd`, `expand`, `env`, `exit_code` |
| EOF message in 3 separate `ft_putstr_fd` | Message concatenated |

---

### 4. exec/execute.c

| Original | Refactoredv3 |
|----------|---------------|
| Does not process heredocs in executor | **New line**: `process_heredocs(cmd, shell->env_vars, shell->exit_code);` called before execution |
| Heredocs processed elsewhere | Heredocs now processed before any execution |

---

### 5. exec/child.c

| Original | Refactoredv3 |
|----------|---------------|
| `handle_exec_path()` checks `ft_strchr` and then calls `find_path` | Separated into `resolve_cmd_path()` + `handle_exec_path()` |
| Empty args check logic in `child_process()` | Extracted to `child_exec()` |
| Same code, but more modularized | Separation of responsibilities |

---

### 6. main.c

| Original | Refactoredv3 |
|----------|---------------|
| Everything in `main()` (~95 lines) | Separated into: `shell_loop()`, `init_shell()`, `get_input()` |
| `is_comment_line()` inline | Kept in `main_utils.c` |
| `print_env_debug()` (debug function) | Removed |
| Does not validate quotes | New function `has_unclosed_quotes()` |
| Only checks comments | New `validate_line()` that checks comments + unclosed quotes |

---

### 7. minishell.h (Header)

Added declarations:

- `build_full_word`
- `parse_redir_in`
- `parse_redir_out`
- `validate_line`
- `process_line`
- New struct `t_hd_ctx` for heredoc context
- New heredoc functions: `heredoc_has_quotes`, `heredoc_remove_quotes`, `heredoc_gen_temp_filename`, `heredoc_read_line`

---

### 8. Makefile

| Original | Refactoredv3 |
|----------|---------------|
| `CFLAGS = -Wall -Wextra -Werror -g` | `CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address,undefined` |
| No suppressions | Automatically generates `readline.supp` |
| Basic targets | New targets: `valgrind`, `valgrind_fd` |

---

## Architectural Summary

### 1. Modularization
Functions extracted to separate files:
- `main_utils.c` - Main loop and line validation
- `lexer_word.c` - Word building in lexer
- `parser_redir.c` - Redirection parsing
- `heredoc_utils.c` - Heredoc utilities
- `utils2.c` - Additional utilities

### 2. Non-TTY Support
Refactoredv3 works in both interactive mode (tty) and in pipes/scripts:
- Uses `isatty(STDIN_FILENO)` to detect terminal mode
- Falls back to `read_line()` from `utils2.c` when not in terminal
- Heredoc also supports non-tty mode

### 3. Sanitizers
Added for debugging memory errors:
- AddressSanitizer (`-fsanitize=address`)
- UndefinedBehaviorSanitizer (`-fsanitize=undefined`)
- Automatic generation of `readline.supp` for valgrind

### 4. Heredocs
Better organization:
- New `t_hd_ctx` struct to pass context
- Support for non-tty heredoc input
- Cleaner code with separate utilities

### 5. Input Validation
New validation before processing:
- Comment lines detection
- Unclosed quotes detection
- Proper exit code (2) for syntax errors

---

## Key Differences in Execution Flow

### Original
```
main() -> get line -> check comment -> lexer -> parser -> executor -> execute
```

### Refactoredv3
```
main() -> init_shell() -> shell_loop()
         -> get_input() -> validate_line() (comment + quotes check)
         -> lexer -> parser -> executor
         -> process_heredocs() (BEFORE execution)
         -> execute
```

---

## Compilation

### Original
```bash
make
```

### Refactoredv3
```bash
make                    # Compile with sanitizers
make valgrind           # Run with valgrind
make valgrind_fd       # Run with valgrind + fd tracking
```

---

*Generated on 2026-02-21*
