/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 23:00:00 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/20 23:00:00 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_comment_line(char *line)
{
	int	i;

	if (!line)
		return (0);
	i = 0;
	while (line[i] && is_space(line[i]))
		i++;
	return (line[i] == '#');
}

static int	has_unclosed_quotes(char *line)
{
	int		i;
	char	quote;

	i = 0;
	while (line[i])
	{
		if (line[i] == '\'' || line[i] == '"')
		{
			quote = line[i++];
			while (line[i] && line[i] != quote)
				i++;
			if (!line[i])
				return (1);
			i++;
		}
		else
			i++;
	}
	return (0);
}

int	validate_line(char *line, t_shell *shell)
{
	if (is_comment_line(line))
	{
		shell->exit_code = 0;
		free(line);
		return (1);
	}
	if (has_unclosed_quotes(line))
	{
		ft_putendl_fd("minishell: syntax error: unclosed quotes", 2);
		shell->exit_code = 2;
		free(line);
		return (1);
	}
	return (0);
}

void	process_line(char *line, t_shell *shell)
{
	t_token	*tokens;
	t_cmd	*cmds;

	tokens = lexer(line, shell);
	if (tokens)
	{
		shell->s_tokens = tokens;
		cmds = parser(tokens, shell);
		if (cmds)
		{
			shell->s_cmds = cmds;
			executor(cmds, shell);
			free_cmds(cmds);
			shell->s_cmds = NULL;
		}
		free_tokens(tokens);
		shell->s_tokens = NULL;
	}
}
