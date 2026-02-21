/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_router.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 14:47:16 by abroslav          #+#    #+#             */
/*   Updated: 2026/01/31 17:37:46 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(char **args)
{
	if (!args || !args[0])
		return (0);
	if (!ft_strncmp(args[0], "echo", 5))
		return (1);
	if (!ft_strncmp(args[0], "pwd", 4))
		return (1);
	if (!ft_strncmp(args[0], "env", 4))
		return (args[1] == NULL);
	if (!ft_strncmp(args[0], "exit", 5))
		return (1);
	if (!ft_strncmp(args[0], "cd", 3))
		return (1);
	if (!ft_strncmp(args[0], "export", 7))
		return (1);
	if (!ft_strncmp(args[0], "unset", 6))
		return (1);
	return (0);
}

int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	char	**args;

	args = cmd->args;
	if (!args || !args[0])
		return (0);
	if (!ft_strncmp(args[0], "echo", 5))
		return (ft_echo(args));
	if (!ft_strncmp(args[0], "pwd", 4))
		return (ft_pwd(shell));
	if (!ft_strncmp(args[0], "env", 4))
		return (ft_env(args, shell->env_vars));
	if (!ft_strncmp(args[0], "exit", 5))
		return (ft_exit(args, shell));
	if (!ft_strncmp(args[0], "cd", 3))
		return (ft_cd(args, &shell->env_vars));
	if (!ft_strncmp(args[0], "export", 7))
		return (ft_export(args, &shell->env_vars));
	if (!ft_strncmp(args[0], "unset", 6))
		return (ft_unset(args, &shell->env_vars));
	return (0);
}
