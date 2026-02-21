/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_errors.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 20:57:23 by abroslav          #+#    #+#             */
/*   Updated: 2026/02/16 16:56:06 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*Write the error string in a single call to avoid diff msg from  diff process*/
static void	print_error_message(char *cmd, char *msg)
{
	char	*part;
	char	*full;
	char	*with_nl;

	part = ft_strjoin("minishell: ", cmd);
	if (!part)
		return ;
	full = ft_strjoin(part, msg);
	free(part);
	if (!full)
		return ;
	with_nl = ft_strjoin(full, "\n");
	free(full);
	if (!with_nl)
		return ;
	write(2, with_nl, ft_strlen(with_nl));
	free(with_nl);
}

/* Gere as mensagens de erro de execução e encerra o processo filho.
 * 1. Se tiver '/' e for diretório -> 126 "Is a directory"
 * 2. Se tiver '/' e não existir -> 127 "No such file or directory"
 * 3. Se não tiver '/' e não estiver no PATH -> 127 "command not found"
 * 4. Se não tiver permissão de execução -> 126 "Permission denied" */
void	execution_error(char *cmd, int code, t_shell *shell)
{
	struct stat	path_stat;

	if (cmd && ft_strchr(cmd, '/'))
	{
		if (stat(cmd, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
		{
			print_error_message(cmd, ": Is a directory");
			cleanup_exit_child(shell, 126);
		}
		if (code == 127)
		{
			print_error_message(cmd, ": No such file or directory");
			cleanup_exit_child(shell, 127);
		}
	}
	if (code == 127)
		print_error_message(cmd, ": command not found");
	else if (code == 126)
		print_error_message(cmd, ": Permission denied");
	else
		print_error_message(cmd, strerror(errno));
	cleanup_exit_child(shell, code);
}
