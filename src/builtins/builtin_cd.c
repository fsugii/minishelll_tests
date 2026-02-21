/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 13:45:03 by abroslav          #+#    #+#             */
/*   Updated: 2026/02/01 15:58:36 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*strerror(errno) prints the dir. not found and 
 * getcwd error for current delete dir.*/
static void	print_cd_error(char *arg, int is_getcwd)
{
	ft_putstr_fd("minishell: cd: ", 2);
	if (is_getcwd)
	{
		ft_putstr_fd("error retrieving current directory: getcwd: ", 2);
		ft_putstr_fd("cannot access parent directories: ", 2);
		ft_putendl_fd("No such file or directory", 2);
	}
	else
	{
		if (arg)
		{
			ft_putstr_fd(arg, 2);
			ft_putstr_fd(": ", 2);
		}
		ft_putendl_fd(strerror(errno), 2);
	}
}

/*Update env with export - update PWD e OLDPWD in the env
 * Uses ft_export logic to ensure the matrix is correctly reallocated*/
static void	update_w_export(char ***env, char *key, char *value)
{
	char	*tmp;
	char	*final_str;
	char	*args[3];

	if (!value)
		return ;
	tmp = ft_strjoin(key, "=");
	final_str = ft_strjoin(tmp, value);
	free(tmp);
	args[0] = "export";
	args[1] = final_str;
	args[2] = NULL;
	ft_export(args, env);
	free(final_str);
}

/*If no args go to HOME else go to args[1] - Send error to FD 2 
 * Handles 'cd', 'cd ~' & 'cd -'.							*/
static void	*get_target_dir(char **args, char **env)
{
	char	*path;

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
	{
		path = get_env_value(env, "HOME");
		if (!path)
			ft_putendl_fd("minishell: cd: HOME not set", 2);
		return (path);
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		path = get_env_value(env, "OLDPWD");
		if (!path)
			ft_putendl_fd("minishell: cd: OLDPWD not set", 2);
		else
			ft_putendl_fd(path, 1);
		return (path);
	}
	return (args[1]);
}

/*manage PWD updates and deleted directory*/
static void	manage_pwd(char ***env, char *old_pwd, char *target)
{
	char	cwd[PATH_MAX];
	char	*ghost;
	char	*tmp;

	update_w_export(env, "OLDPWD", old_pwd);
	if (getcwd(cwd, PATH_MAX))
		update_w_export(env, "PWD", cwd);
	else
	{
		print_cd_error(NULL, 1);
		tmp = ft_strjoin(old_pwd, "/");
		ghost = ft_strjoin(tmp, target);
		update_w_export(env, "PWD", ghost);
		free(tmp);
		free(ghost);
	}
}

/*Change dir. saving the current and the old working dir.*/
int	ft_cd(char **args, char ***env)
{
	char	*target_dir;
	char	*old_pwd;

	if (args[1] && args[1][0] == '\0')
		return (0);
	if (args[1] && args[2])
	{
		ft_putendl_fd("minishell: cd: too many arguments", 2);
		return (1);
	}
	old_pwd = get_env_value(*env, "PWD");
	target_dir = get_target_dir(args, *env);
	if (!target_dir)
		return (1);
	if (chdir(target_dir) != 0)
	{
		print_cd_error(target_dir, 0);
		return (1);
	}
	manage_pwd(env, old_pwd, target_dir);
	return (0);
}
