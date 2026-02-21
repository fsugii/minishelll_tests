/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 15:17:07 by abroslav          #+#    #+#             */
/*   Updated: 2026/02/16 17:30:59 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*resolve_cmd_path(t_cmd *cmd, t_shell *shell, int *malloced)
{
	char	*path;

	*malloced = 0;
	if (ft_strchr(cmd->args[0], '/'))
	{
		if (access(cmd->args[0], F_OK) != 0)
			execution_error(cmd->args[0], 127, shell);
		return (cmd->args[0]);
	}
	path = find_path(cmd->args[0], shell->env_vars);
	*malloced = 1;
	if (!path)
		execution_error(cmd->args[0], 127, shell);
	return (path);
}

static void	handle_exec_path(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	int		path_malloced;
	int		err_code;

	path = resolve_cmd_path(cmd, shell, &path_malloced);
	execve(path, cmd->args, shell->env_vars);
	if (errno == EACCES)
		err_code = 126;
	else
		err_code = 1;
	if (path_malloced)
		free(path);
	execution_error(cmd->args[0], err_code, shell);
}

void	handle_pipes(t_cmd *cmd, int fd_in, int *fd_pipe)
{
	if (fd_in != -1)
	{
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	if (cmd->next)
	{
		close(fd_pipe[0]);
		dup2(fd_pipe[1], STDOUT_FILENO);
		close(fd_pipe[1]);
	}
}

static void	child_exec(t_cmd *cmd, t_shell *shell)
{
	int	status;

	if (!cmd->args || !cmd->args[0])
		cleanup_exit_child(shell, 0);
	if (cmd->args[0][0] == '\0')
		execution_error(cmd->args[0], 127, shell);
	if (is_builtin(cmd->args))
	{
		status = exec_builtin(cmd, shell);
		cleanup_exit_child(shell, status);
	}
	handle_exec_path(cmd, shell);
}

void	child_process(t_cmd *cmd, int fd_in, int *fd_pipe, t_shell *shell)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	handle_pipes(cmd, fd_in, fd_pipe);
	if (handle_redirection(cmd) != 0)
		cleanup_exit_child(shell, 1);
	child_exec(cmd, shell);
}
