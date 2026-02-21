/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 14:42:40 by abroslav          #+#    #+#             */
/*   Updated: 2026/01/29 14:47:10 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	exit_numeric_error(char *arg, t_shell *shell)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd(": numeric argument required", 2);
	free_all(shell);
	exit(2);
}

int	ft_exit(char **args, t_shell *shell)
{
	long long	res;
	int			i;

	ft_putendl_fd("exit", 1);
	i = 1;
	if (args[i] && ft_strcmp(args[i], "--") == 0)
		i++;
	if (!args[i])
	{
		free_all(shell);
		exit(shell->exit_code);
	}
	if (ft_atoll_overflow(args[i], &res))
		exit_numeric_error(args[i], shell);
	if (args[i + 1])
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		shell->exit_code = 1;
		return (1);
	}
	shell->exit_code = (unsigned char)res;
	free_all(shell);
	exit(shell->exit_code);
}
