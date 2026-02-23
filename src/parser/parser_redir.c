/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 23:00:00 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/20 23:00:00 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parse_redir_in(t_cmd *current_cmd, t_token **tokens)
{
	t_redir	*redir;
	char	*tmp;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return ;
	if ((*tokens)->type == TK_HEREDOC)
		redir->type = REDIR_HEREDOC;
	else
		redir->type = REDIR_IN;
	*tokens = (*tokens)->next;
	if (*tokens && (*tokens)->type == TK_WORD)
	{
		if (redir->type == REDIR_HEREDOC && (*tokens)->no_expand)
		{
			tmp = ft_strjoin((*tokens)->value, "\"");
			redir->target = ft_strjoin("\"", tmp);
			free(tmp);
		}
		else
			redir->target = ft_strdup((*tokens)->value);
	}
	else
		redir->target = NULL;
	redir->next = NULL;
	redir_add_back(&current_cmd->redirs, redir);
}

void	parse_redir_out(t_cmd *current_cmd, t_token **tokens)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return ;
	if ((*tokens)->type == TK_APPEND)
		redir->type = REDIR_APPEND;
	else
		redir->type = REDIR_OUT;
	*tokens = (*tokens)->next;
	if (*tokens && (*tokens)->type == TK_WORD)
		redir->target = ft_strdup((*tokens)->value);
	else
		redir->target = NULL;
	redir->next = NULL;
	redir_add_back(&current_cmd->redirs, redir);
}
