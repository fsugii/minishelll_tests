/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 03:15:10 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/02 13:54:11 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_var_value(char *var_name, char **env)
{
	char	*value;

	value = get_env_value(env, var_name);
	if (value)
		return (ft_strdup(value));
	return (ft_strdup(""));
}

static char	*expand_var_name(char *str, int *i, char **env)
{
	int		start;
	char	*var_name;
	char	*var_value;

	start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	if (*i == start)
	{
		if (str[*i])
			(*i)++;
		return (ft_strdup(""));
	}
	var_name = ft_substr(str, start, *i - start);
	var_value = get_var_value(var_name, env);
	return (free(var_name), var_value);
}

static char	*extract_and_expand_var(char *str, int *i, char **env,
		int last_exit)
{
	(*i)++;
	if (str[*i] == '?' || str[*i] == '$')
	{
		return (special_expand_params(str[(*i)++], last_exit));
	}
	if (!str[*i] || (!ft_isalnum(str[*i]) && str[*i] != '_'))
		return (ft_strdup("$"));
	if (ft_isdigit(str[*i]))
	{
		(*i)++;
		return (ft_strdup(""));
	}
	return (expand_var_name(str, i, env));
}

char	*expand_vars(char *str, char **env, int last_exit)
{
	char	*result;
	char	*var_value;
	char	tmp[2];
	int		i;

	if (!str)
		return (NULL);
	result = ft_strdup("");
	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1])
		{
			var_value = extract_and_expand_var(str, &i, env, last_exit);
			result = join_and_free(result, var_value);
		}
		else
		{
			tmp[0] = str[i];
			tmp[1] = '\0';
			result = join_and_free(result, ft_strdup(tmp));
			i++;
		}
	}
	return (result);
}

void	expand_tokens(t_token *tokens, char **env, int last_exit)
{
	char	*expanded;

	while (tokens)
	{
		if (tokens->type == TK_WORD && !tokens->no_expand)
		{
			expanded = expand_vars(tokens->value, env, last_exit);
			if (expanded)
			{
				free(tokens->value);
				tokens->value = expanded;
			}
		}
		tokens = tokens->next;
	}
}
