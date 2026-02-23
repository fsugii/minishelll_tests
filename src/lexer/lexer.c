/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 00:30:25 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/18 22:01:25 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_redir_out(t_token **tokens, char *line, int *i)
{
	if (line[*i + 1] == '>')
	{
		token_add_back(tokens, new_token(">>", TK_APPEND));
		(*i)++;
	}
	else
		token_add_back(tokens, new_token(">", TK_REDIR_OUT));
}

static void	handle_redir_in(t_token **tokens, char *line, int *i)
{
	if (line[*i + 1] == '<')
	{
		token_add_back(tokens, new_token("<<", TK_HEREDOC));
		(*i)++;
	}
	else
		token_add_back(tokens, new_token("<", TK_REDIR_IN));
}

static void	process_char(t_token **tokens, char *line, int *i,
		t_shell *shell)
{
	char	*word;
	t_token	*tok;
	int		quoted;

	if (is_space(line[*i]))
		return ;
	else if (line[*i] == '|')
		token_add_back(tokens, new_token("|", TK_PIPE));
	else if (line[*i] == '<')
		handle_redir_in(tokens, line, i);
	else if (line[*i] == '>')
		handle_redir_out(tokens, line, i);
	else
	{
		word = build_full_word(line, i, shell, &quoted);
		tok = new_token(word, TK_WORD);
		if (tok && quoted)
			tok->no_expand = 1;
		token_add_back(tokens, tok);
		free(word);
		(*i)--;
	}
}

t_token	*lexer(char *line, t_shell *shell)
{
	t_token	*tokens;
	int		i;

	if (!line)
		return (NULL);
	tokens = NULL;
	i = 0;
	while (line[i])
	{
		process_char(&tokens, line, &i, shell);
		i++;
	}
	return (tokens);
}
