/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 20:39:41 by fshiniti          #+#    #+#             */
/*   Updated: 2026/01/23 02:30:50 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_space(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

int	is_special(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

t_token	*new_token(char *tk_str, t_token_type type)
{
	t_token	*node;

	node = malloc(sizeof(t_token));
	if (!node)
		return (NULL);
	node->value = ft_strdup(tk_str);
	if (!node->value)
	{
		free(node);
		return (NULL);
	}
	node->type = type;
	node->no_expand = 0;
	node->in_dquotes = 0;
	node->next = NULL;
	return (node);
}

void	token_add_back(t_token **tokens, t_token *new_node)
{
	t_token	*tmp;

	if (!tokens || !new_node)
		return ;
	if (!*tokens)
	{
		*tokens = new_node;
		return ;
	}
	tmp = *tokens;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_node;
}

char	*join_and_free(char *s1, char *s2)
{
	char	*result;

	if (!s1 || !s2)
		return (NULL);
	result = ft_strjoin(s1, s2);
	free(s1);
	free(s2);
	return (result);
}
