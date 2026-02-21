/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 10:56:48 by abroslav          #+#    #+#             */
/*   Updated: 2026/01/30 21:08:01 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*Prints a sigle env line in the format: declare -x KEY=VALUE*/
static void	print_export_line(char *env_var)
{
	int	i;

	i = 0;
	if (!env_var || ft_strncmp(env_var, "_=", 2) == 0)
		return ;
	ft_putstr_fd("declare -x ", 1);
	while (env_var[i] && env_var[i] != '=')
		write(1, &env_var[i++], 1);
	if (env_var[i] == '=')
	{
		write(1, "=\"", 2);
		i++;
		while (env_var[i])
			write(1, &env_var[i++], 1);
		write(1, "\"", 1);
	}
	write(1, "\n", 1);
}

/*Bubble sort to order the pointers matrix alphabetically*/
static void	sort_env_matrix(char **matrix, int len)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (i < len - 1)
	{
		j = i + 1;
		while (j < len)
		{
			if (ft_strcmp(matrix[i], matrix[j]) > 0)
			{
				tmp = matrix[i];
				matrix[i] = matrix[j];
				matrix[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

/*Tmp copy of env to sort and print them*/
void	show_export_list(char **env)
{
	char	**sorted_env;
	int		len;
	int		i;

	len = get_matrix_len(env);
	sorted_env = malloc(sizeof(char *) * (len + 1));
	if (!sorted_env)
		return ;
	i = 0;
	while (i < len)
	{
		sorted_env[i] = env[i];
		i++;
	}
	sorted_env[i] = NULL;
	sort_env_matrix(sorted_env, len);
	i = 0;
	while (i < len)
		print_export_line(sorted_env[i++]);
	free(sorted_env);
}
