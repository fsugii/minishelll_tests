/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 23:00:00 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/24 12:00:00 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Wraps the delimiter in quotes so heredoc_has_quotes() detects it
** and disables variable expansion inside the heredoc body.
** Called when the delimiter token came from single quotes (no_expand=1).
*/
static char	*quoted_target(char *value)
{
	char	*tmp;
	char	*result;

	tmp = ft_strjoin("'", value);
	result = ft_strjoin(tmp, "'");
	free(tmp);
	return (result);
}

static t_redir	*new_redir(t_redir_type type)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->target = NULL;
	redir->next = NULL;
	return (redir);
}

void	parse_redir_in(t_cmd *current_cmd, t_token **tokens)
{
	t_redir			*redir;
	t_redir_type	type;

	if ((*tokens)->type == TK_HEREDOC)
		type = REDIR_HEREDOC;
	else
		type = REDIR_IN;
	redir = new_redir(type);
	if (!redir)
		return ;
	*tokens = (*tokens)->next;
	if (*tokens && (*tokens)->type == TK_WORD)
	{
		if (type == REDIR_HEREDOC && (*tokens)->no_expand)
			redir->target = quoted_target((*tokens)->value);
		else
			redir->target = ft_strdup((*tokens)->value);
	}
	redir_add_back(&current_cmd->redirs, redir);
}

void	parse_redir_out(t_cmd *current_cmd, t_token **tokens)
{
	t_redir			*redir;
	t_redir_type	type;

	if ((*tokens)->type == TK_APPEND)
		type = REDIR_APPEND;
	else
		type = REDIR_OUT;
	redir = new_redir(type);
	if (!redir)
		return ;
	*tokens = (*tokens)->next;
	if (*tokens && (*tokens)->type == TK_WORD)
		redir->target = ft_strdup((*tokens)->value);
	redir_add_back(&current_cmd->redirs, redir);
}
