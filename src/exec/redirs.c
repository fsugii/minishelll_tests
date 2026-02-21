/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 15:17:44 by abroslav          #+#    #+#             */
/*   Updated: 2026/02/06 18:00:28 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*Opens the file and redirects the FD. Return 1 on error.
 * Bash shows "ambiguous redirect" for empty targets. */
static int	apply_redir(char *file, int flags, int std_fd)
{
	int	fd;

	if (!file || file[0] == '\0')
	{
		ft_putstr_fd("minishell: ", 2);
		if (!file)
			ft_putendl_fd("syntax error near unexpected token", 2);
		else
			ft_putendl_fd(": No such file or directory", 2);
		return (1);
	}
	fd = open(file, flags, 0644);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(file);
		return (1);
	}
	if (dup2(fd, std_fd) == -1)
	{
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int	apply_heredoc(t_cmd *cmd)
{
	if (cmd->heredoc_fd < 0)
		return (1);
	if (dup2(cmd->heredoc_fd, STDIN_FILENO) == -1)
		return (1);
	close(cmd->heredoc_fd);
	return (0);
}

/*Iterate through the comds list
 * Open files and redirects input/output using dup2
 * Break and returns 1 if any redir. fails*/
int	handle_redirection(t_cmd *cmd)
{
	t_redir	*redir;
	int		status;

	if (!cmd)
		return (0);
	redir = cmd->redirs;
	while (redir)
	{
		status = 0;
		if (redir->type == REDIR_IN)
			status = apply_redir(redir->target, O_RDONLY, STDIN_FILENO);
		else if (redir->type == REDIR_OUT)
			status = apply_redir(redir->target, O_WRONLY | O_CREAT | \
O_TRUNC, STDOUT_FILENO);
		else if (redir->type == REDIR_APPEND)
			status = apply_redir(redir->target, O_WRONLY | O_CREAT | \
O_APPEND, STDOUT_FILENO);
		else if (redir->type == REDIR_HEREDOC)
			status = apply_heredoc(cmd);
		if (status != 0)
			return (1);
		redir = redir->next;
	}
	return (0);
}
