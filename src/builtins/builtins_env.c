/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 16:09:15 by abroslav          #+#    #+#             */
/*   Updated: 2026/01/31 17:40:29 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_key_match(char *env_var, char *key)
{
	int	len;

	len = ft_strlen(key);
	if (ft_strncmp(env_var, key, len) == 0
		&& (env_var[len] == '=' || env_var[len] == '\0'))
		return (1);
	return (0);
}

static void	remove_env_var(char *key, char ***env)
{
	char	**new_env;
	char	**old;
	int		i;
	int		j;

	old = *env;
	new_env = malloc(sizeof(char *) * (get_matrix_len(old) + 1));
	if (!new_env)
		return ;
	i = 0;
	j = 0;
	while (old[i])
	{
		if (is_key_match(old[i], key))
			free(old[i]);
		else
			new_env[j++] = old[i];
		i++;
	}
	new_env[j] = NULL;
	free(old);
	*env = new_env;
}

int	ft_unset(char **args, char ***env)
{
	int		i;

	if (!args[1] || !(*env))
		return (0);
	i = 1;
	while (args[i])
	{
		remove_env_var(args[i], env);
		i++;
	}
	return (0);
}

int	ft_export(char **args, char ***env)
{
	int		i;
	int		status;

	status = 0;
	if (!args[1])
	{
		show_export_list(*env);
		return (0);
	}
	i = 1;
	while (args[i])
	{
		if (!is_valid_key(args[i]))
		{
			ft_putstr_fd("minishell: export: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putendl_fd("': not a valid identifier", 2);
			status = 1;
		}
		else
			update_env(args[i], env);
		i++;
	}
	return (status);
}

int	ft_env(char **args, char **env)
{
	int	i;

	(void)args;
	if (!env)
		return (1);
	i = 0;
	while (env[i])
	{
		if (ft_strchr(env[i], '='))
		{
			ft_putstr_fd(env[i], 1);
			ft_putstr_fd("\n", 1);
		}
		i++;
	}
	return (0);
}
