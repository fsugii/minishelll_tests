/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 22:59:11 by fshiniti          #+#    #+#             */
/*   Updated: 2026/01/20 23:29:31 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	write_hd_line(t_hd_ctx *ctx, char *line)
{
	char	*expanded;

	if (ctx->expand)
	{
		expanded = expand_vars(line, ctx->env, ctx->exit_code);
		write(ctx->fd, expanded, ft_strlen(expanded));
		free(expanded);
	}
	else
		write(ctx->fd, line, ft_strlen(line));
	write(ctx->fd, "\n", 1);
}

static void	read_heredoc_lines(t_hd_ctx *ctx, char *delimiter)
{
	char	*line;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			line = readline("> ");
		else
			line = heredoc_read_line();
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-document delimited ", 2);
			ft_putstr_fd("by end-of-file (wanted '", 2);
			ft_putstr_fd(delimiter, 2);
			ft_putstr_fd("')\n", 2);
			break ;
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write_hd_line(ctx, line);
		free(line);
	}
}

int	handle_heredoc(char *delimiter, char **env, int last_exit)
{
	t_hd_ctx	ctx;
	char		*clean_delimiter;
	char		*temp_file;
	int			fd;

	ctx.expand = !heredoc_has_quotes(delimiter);
	ctx.env = env;
	ctx.exit_code = last_exit;
	clean_delimiter = heredoc_remove_quotes(delimiter);
	temp_file = heredoc_gen_temp_filename();
	ctx.fd = open(temp_file, O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if (ctx.fd < 0)
	{
		perror("minishell: heredoc");
		free(clean_delimiter);
		free(temp_file);
		return (-1);
	}
	read_heredoc_lines(&ctx, clean_delimiter);
	free(clean_delimiter);
	close(ctx.fd);
	fd = open(temp_file, O_RDONLY);
	unlink(temp_file);
	free(temp_file);
	return (fd);
}

void	process_heredocs(t_cmd *cmds, char **env, int last_exit)
{
	t_cmd	*cmd;
	t_redir	*redir;

	cmd = cmds;
	while (cmd)
	{
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC)
				cmd->heredoc_fd = handle_heredoc(redir->target, env, last_exit);
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}
