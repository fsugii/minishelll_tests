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

static int	read_heredoc_lines(t_hd_ctx *ctx, char *delimiter)
{
	char	*line;

	while (1)
	{
		g_last_signal = 0;
		if (isatty(STDIN_FILENO))
			line = readline("> ");
		else
			line = heredoc_read_line();
		if (!line)
		{
			if (g_last_signal == SIGINT)
				return (1);
			heredoc_eof_warning(delimiter);
			break ;
		}
		if (ft_strcmp(line, delimiter) == 0)
			return (free(line), 0);
		write_hd_line(ctx, line);
		free(line);
	}
	return (0);
}

static int	heredoc_read(t_hd_ctx *ctx, char *delim)
{
	int	stdin_bak;
	int	ret;

	stdin_bak = dup(STDIN_FILENO);
	setup_signals_heredoc();
	ret = read_heredoc_lines(ctx, delim);
	if (ret)
	{
		dup2(stdin_bak, STDIN_FILENO);
		close(stdin_bak);
		setup_signals();
		return (1);
	}
	close(stdin_bak);
	setup_signals();
	return (0);
}

int	handle_heredoc(char *delimiter, char **env, int last_exit)
{
	t_hd_ctx	ctx;
	char		*clean_delim;
	char		*tmp;
	int			fd;

	ctx.expand = !heredoc_has_quotes(delimiter);
	ctx.env = env;
	ctx.exit_code = last_exit;
	clean_delim = heredoc_remove_quotes(delimiter);
	tmp = heredoc_gen_temp_filename();
	ctx.fd = open(tmp, O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if (ctx.fd < 0)
		return (perror("minishell: heredoc"),
			free(clean_delim), free(tmp), -1);
	if (heredoc_read(&ctx, clean_delim))
	{
		close(ctx.fd);
		unlink(tmp);
		return (free(clean_delim), free(tmp), -1);
	}
	close(ctx.fd);
	fd = open(tmp, O_RDONLY);
	return (unlink(tmp), free(tmp), free(clean_delim), fd);
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
