/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_info.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 14:45:52 by abroslav          #+#    #+#             */
/*   Updated: 2026/02/04 17:05:52 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_n_flag(char *str)
{
	int	i;

	if (!str || str[0] != '-' || str[1] != 'n')
		return (0);
	i = 1;
	while (str[i])
	{
		if (str[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

int	ft_echo(char **args)
{
	int	i;
	int	n_flag;

	i = 1;
	n_flag = 0;
	while (args[i] && is_valid_n_flag(args[i]))
	{
		n_flag = 1;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], 1);
		if (args[i + 1] != NULL)
			write(1, " ", 1);
		i++;
	}
	if (!n_flag)
		ft_putendl_fd("", 1);
	return (0);
}

int	ft_pwd(t_shell *shell)
{
	char	*cwd;
	char	buf[PATH_MAX];

	cwd = get_env_value(shell->env_vars, "PWD");
	if (cwd && *cwd)
	{
		ft_putendl_fd(cwd, 1);
		return (0);
	}
	if (getcwd(buf, PATH_MAX))
	{
		ft_putendl_fd(buf, 1);
		return (0);
	}
	ft_putendl_fd("minishell: pwd: error retrieving current directory", 2);
	return (1);
}
