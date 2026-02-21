/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:19:35 by fshiniti          #+#    #+#             */
/*   Updated: 2026/01/28 13:08:20 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	shlvl_update(char ***env)
{
	char	*tmp_shlvl;
	char	*new_shlvl;
	int		lvl;

	tmp_shlvl = get_env_value(*env, "SHLVL");
	if (!tmp_shlvl)
		lvl = 1;
	else
	{
		lvl = ft_atoi(tmp_shlvl) + 1;
		if (lvl > 999)
		{
			ft_putstr_fd("minishell: warning: shell level", 2);
			ft_putendl_fd(" too high, resetting to 1", 2);
			lvl = 1;
		}
		else if (lvl < 0)
			lvl = 0;
	}
	tmp_shlvl = ft_itoa(lvl);
	new_shlvl = ft_strjoin("SHLVL=", tmp_shlvl);
	update_env(new_shlvl, env);
	free(tmp_shlvl);
	free(new_shlvl);
}

static int	count_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
		i++;
	return (i);
}

char	**copy_env(char **envp)
{
	char	**new_env;
	int		i;
	int		len;

	len = count_env(envp);
	new_env = malloc(sizeof(char *) * (len + 1));
	if (!new_env)
		return (NULL);
	i = 0;
	while (i < len)
	{
		new_env[i] = ft_strdup(envp[i]);
		if (!new_env[i])
		{
			while (--i >= 0)
				free(new_env[i]);
			free(new_env);
			return (NULL);
		}
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

void	free_env(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
		free (env[i++]);
	free(env);
}
