/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 20:44:55 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/16 17:09:31 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <readline/readline.h> // rl*
# include <readline/history.h>	// add_history
# include <stdlib.h>			// malloc, free, exit, getenv
# include <unistd.h>			// fork, execve, pipe, dup, dup2, chdir, getcwd,
								// access, read, write, isatty, unlink
# include <stdio.h>				// printf, perror
# include <fcntl.h>				// open
# include <sys/wait.h>			// wait, waitpid, wait3, wait4
# include <signal.h>			// signal, sigaction, sigemptyset, segaddset, 
								// kill
# include <errno.h>				//errno
# include <string.h>			//strerror
# include <limits.h>			//atoll
# include <sys/types.h>			//stat struct
# include <sys/stat.h>			//permissoes de ficheiro - erro 126 - 127

# ifndef PATH_MAX				//pwd
#  define PATH_MAX 4096
# endif

extern int	g_last_signal;

typedef enum e_token_type
{
	TK_WORD,					// Words, Arguments
	TK_PIPE,					// |
	TK_REDIR_IN,				// <
	TK_REDIR_OUT,				// >
	TK_APPEND,					// >>
	TK_HEREDOC,					// <<
}	t_token_type;

typedef enum e_redir_type
{
	REDIR_IN,					// <
	REDIR_OUT,					// >
	REDIR_APPEND,				// >>
	REDIR_HEREDOC,				// <<
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;		// Redirection type
	char			*target;	// File name or delimiter
	struct s_redir	*next;		// Pointer for next redirection
}	t_redir;

typedef struct s_token
{
	t_token_type	type;		// Token value
	char			*value;		// Type of token
	int				no_expand;	// Expand or not
	int				in_dquotes;	// Came from double quotes?
	struct s_token	*next;		// Pointer for next token
}	t_token;

typedef struct s_cmd
{
	char			**args;		// Command Args
	t_redir			*redirs;	// Redirects list
	char			**limits;	// Heredoc delimiters
	int				heredoc_fd;	// FD for heredoc
	struct s_cmd	*next;		// Next command in pipe
}	t_cmd;

typedef struct s_shell
{
	char			**cmd_args;		// Command arguments
	char			**env_vars;		// Environment variables
	char			**export_vars;	// Exported variables
	char			**cmd_history;	// Commands history
	int				exit_code;		// Exit code
	t_token			*s_tokens;		//Para limpeza depois do exit
	t_cmd			*s_cmds;
}	t_shell;

/* ===LEXER=== */
t_token	*lexer(char *input, t_shell *shell);
t_token	*new_token(char *value, t_token_type type);
void	token_add_back(t_token **list, t_token *new_node);
int		is_space(char c);
int		is_special(char c);
char	*join_and_free(char *s1, char *s2);
char	*build_full_word(char *line, int *i, t_shell *shell, int *quoted);

/* ===EXPANDER=== */
char	*expand_vars(char *str, char **env, int last_exit);
void	expand_tokens(t_token *tokens, char **env, int last_exit);

/* ===PARSER=== */
t_cmd	*parser(t_token *tokens, t_shell *shell);
t_cmd	*new_cmd(void);
void	cmd_add_back(t_cmd **list, t_cmd *new_node);
void	cmd_add_arg(t_cmd *cmd, char *arg);
void	redir_add_back(t_redir **list, t_redir *new_node);
void	parse_redir_in(t_cmd *current_cmd, t_token **tokens);
void	parse_redir_out(t_cmd *current_cmd, t_token **tokens);

/* ===UTILS=== */
void	print_tokens(t_token *tok);
void	print_cmds(t_cmd *cmd);
void	free_tokens(t_token *list);
void	free_cmds(t_cmd *cmds);
int		ft_atoll_overflow(const char *str, long long *res);
void	free_all(t_shell *shell);
void	cleanup_exit_child(t_shell *shell, int exit_code);
int		is_valid_key(char *str);
int		ft_strcmp(const char *s1, const char *s2);
int		is_valid_n_flag(char *str);
int		ft_isspace(int c);
char	*special_expand_params(char c, int last_exit);
char	*read_line(void);
int		validate_line(char *line, t_shell *shell);
void	process_line(char *line, t_shell *shell);

/* ===ENV=== */
char	**copy_env(char **envp);
void	free_env(char **env);
char	*get_env_value(char **env, char *key);
int		ft_export(char **args, char ***env);
int		ft_unset(char **args, char ***env);
int		get_matrix_len(char **env);
void	add_var_to_env(char ***env, char *var);
void	update_env(char *arg, char ***env);
void	show_export_list(char **env);
void	shlvl_update(char ***env);

/* ===SIGNALS=== */
void	setup_signals(void);
void	handle_sigint(int sig);
void	setup_signals_execution(void);

/* === EXECUTION === */
void	executor(t_cmd *cmd, t_shell *shell);
void	execute_pipe(t_cmd *cmd, t_shell *shell);
char	*find_path(char *cmd, char **envp);
void	free_tab(char **tab);
void	check_open(char *file, int flags, int std_fd);
void	execution_error(char *cmd, int code, t_shell *shell);
void	handle_pipes(t_cmd *cmd, int fd_in, int *fd_pipe);
int		handle_redirection(t_cmd *cmd);
void	child_process(t_cmd *cmd, int fd_ind, int *fd_pipe, t_shell *shell);

/* === HEREDOC === */
typedef struct s_hd_ctx
{
	int		fd;
	int		expand;
	char	**env;
	int		exit_code;
}	t_hd_ctx;

int		handle_heredoc(char *delimiter, char **env, int last_exit);
void	process_heredocs(t_cmd *cmds, char **env, int last_exit);
int		heredoc_has_quotes(char *delimiter);
char	*heredoc_remove_quotes(char *delimiter);
char	*heredoc_gen_temp_filename(void);
char	*heredoc_read_line(void);

/* === BUILTINS  === */
int		is_builtin(char **args);
int		exec_builtin(t_cmd *cmd, t_shell *shell);
int		ft_echo(char **args);
int		ft_pwd(t_shell *shell);
int		ft_env(char **args, char **env);
int		ft_exit(char **args, t_shell *shell);
int		ft_cd(char **args, char ***env);

#endif
