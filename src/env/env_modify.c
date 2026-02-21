/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_modify.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 15:27:26 by abroslav          #+#    #+#             */
/*   Updated: 2026/01/29 19:48:50 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_key(char *str)
{
	int	i;

	i = 0;
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	while (str[i] && str[i] != '=')
	{
		if (str[i] == '+' && str[i + 1] == '=')
			return (1);
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

void	add_var_to_env(char ***env, char *var)
{
	char	**new_env;
	char	**old;
	int		len;
	int		i;

	old = *env;
	len = get_matrix_len(old);
	new_env = malloc(sizeof(char *) * (len + 2));
	if (!new_env)
		return ;
	i = 0;
	while (old && old[i])
	{
		new_env[i] = old[i];
		i++;
	}
	new_env[i] = ft_strdup(var);
	new_env[i + 1] = NULL;
	if (old)
		free(old);
	*env = new_env;
}

static void	exp_and_append(char *arg, char ***env, char *equal_pos)
{
	char	*key;
	char	*joined_val;
	char	*final_str;
	char	*tmp;

	key = ft_substr(arg, 0, (equal_pos - 1) - arg);
	tmp = get_env_value(*env, key);
	if (tmp)
		joined_val = ft_strjoin(tmp, equal_pos + 1);
	else
		joined_val = ft_strdup(equal_pos + 1);
	tmp = ft_strjoin(key, "=");
	final_str = ft_strjoin(tmp, joined_val);
	free(tmp);
	update_env(final_str, env);
	free(key);
	free(joined_val);
	free(final_str);
}

void	update_env(char *arg, char ***env)
{
	char	*key;
	char	*char_equal;
	char	*unset_args[3];

	char_equal = ft_strchr(arg, '=');
	if (char_equal > arg && *(char_equal - 1) == '+')
	{
		exp_and_append(arg, env, char_equal);
		return ;
	}
	if (char_equal)
	{
		*char_equal = '\0';
		key = ft_strdup(arg);
		*char_equal = '=';
	}
	else
		key = ft_strdup(arg);
	unset_args[0] = "unset";
	unset_args[1] = key;
	unset_args[2] = NULL;
	ft_unset(unset_args, env);
	free(key);
	add_var_to_env(env, arg);
}
