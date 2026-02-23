/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 23:00:00 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/20 23:00:00 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_end_of_word(char c)
{
	return (c == '\0' || is_space(c) || is_special(c));
}

static char	*extract_quoted_content(char *line, int *i)
{
	char	quote;
	int		start;
	char	*content;

	quote = line[*i];
	(*i)++;
	start = *i;
	while (line[*i] && line[*i] != quote)
		(*i)++;
	content = ft_substr(line, start, *i - start);
	if (line[*i] == quote)
		(*i)++;
	return (content);
}

static char	*extract_unquoted(char *line, int *i)
{
	int		start;

	start = *i;
	while (line[*i] && !is_end_of_word(line[*i])
		&& line[*i] != '\'' && line[*i] != '"')
		(*i)++;
	return (ft_substr(line, start, *i - start));
}

static char	*handle_word_part(char *line, int *i, t_shell *shell,
		int *quoted)
{
	char	*part;
	char	*expanded;

	if (line[*i] == '\'')
	{
		*quoted = 1;
		return (extract_quoted_content(line, i));
	}
	else if (line[*i] == '"')
	{
		*quoted = 1;
		part = extract_quoted_content(line, i);
		expanded = expand_vars(part, shell->env_vars, shell->exit_code);
		free(part);
		return (expanded);
	}
	part = extract_unquoted(line, i);
	expanded = expand_vars(part, shell->env_vars, shell->exit_code);
	free(part);
	return (expanded);
}

char	*build_full_word(char *line, int *i, t_shell *shell, int *quoted)
{
	char	*result;
	char	*part;
	int		had_quotes;

	result = ft_strdup("");
	*quoted = 0;
	had_quotes = (line[*i] == '\'' || line[*i] == '"');
	while (line[*i] && !is_end_of_word(line[*i]))
	{
		part = handle_word_part(line, i, shell, quoted);
		result = join_and_free(result, part);
	}
	*quoted = had_quotes;
	return (result);
}
