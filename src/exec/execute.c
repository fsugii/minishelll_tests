/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 11:11:22 by abroslav          #+#    #+#             */
/*   Updated: 2026/02/06 17:41:07 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Save originals FDs fro  terminal.
 * Try redir if it fail -> code 1
 * Execute a single builtin on the parent process and back to the bash*/
static void	exec_single_builtin(t_cmd *cmd, t_shell *shell)
{
	int	tmp_stdin;
	int	tmp_stdout;

	tmp_stdin = dup(STDIN_FILENO);
	tmp_stdout = dup(STDOUT_FILENO);
	if (handle_redirection(cmd) != 0)
		shell->exit_code = 1;
	else
		shell->exit_code = exec_builtin(cmd, shell);
	dup2(tmp_stdin, STDIN_FILENO);
	dup2(tmp_stdout, STDOUT_FILENO);
	close(tmp_stdin);
	close(tmp_stdout);
}

/*Check if the cmnd is a direct attribution (ex: export=a) */
static int	is_right_assignment(char *str)
{
	int	i;

	i = 0;
	if (!str || (!ft_isalpha(*str) && *str != '_'))
		return (0);
	while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
		i++;
	if (str[i] == '=')
		return (1);
	return (0);
}

/*Wait untill all the child process end.
 * The exit_code final always will be the last status from the last cmd*/
static void	wait_children(pid_t last_pid, t_shell *shell)
{
	int	status;
	int	sig;

	if (last_pid <= 0)
		return ;
	waitpid(last_pid, &status, 0);
	if (WIFEXITED(status))
		shell->exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGQUIT)
			ft_putstr_fd("Quit (core dump)\n", 2);
		else if (sig == SIGINT)
			ft_putstr_fd("\n", 2);
		shell->exit_code = 128 + sig;
	}
	while (waitpid(-1, NULL, 0) > 0)
		;
}

/*Main loop for pipelines. Create pipes and forks for each cmd in the list
 *Update the exit code based on the last command's termination status */
void	execute_pipe(t_cmd *cmd, t_shell *shell)
{
	int		fd_pipe[2];
	int		fd_in;
	pid_t	pid;

	fd_in = -1;
	setup_signals_execution();
	while (cmd)
	{
		if (cmd->next && pipe(fd_pipe) == -1)
			return ;
		pid = fork();
		if (pid == 0)
			child_process(cmd, fd_in, fd_pipe, shell);
		if (fd_in != -1)
			close(fd_in);
		if (cmd->next)
		{
			close(fd_pipe[1]);
			fd_in = fd_pipe[0];
		}
		cmd = cmd->next;
	}
	wait_children(pid, shell);
	setup_signals();
}

/*high-level executor that decides the execuion path.
 * If it's a single builtin, it runs int the parent process, otherwise
 * initiates the pipeline logic
 * If redirs fail we dont execute just update exit_code*/
void	executor(t_cmd *cmd, t_shell *shell)
{
	if (!cmd)
		return ;
	process_heredocs(cmd, shell->env_vars, shell->exit_code);
	if (!cmd->next && cmd->args && is_right_assignment(cmd->args[0]))
	{
		update_env(cmd->args[0], &shell->env_vars);
		shell->exit_code = 0;
		return ;
	}
	else if (!cmd->next && cmd->args && is_builtin(cmd->args))
		exec_single_builtin(cmd, shell);
	else
		execute_pipe(cmd, shell);
}
