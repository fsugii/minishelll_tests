/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 09:32:49 by fshiniti          #+#    #+#             */
/*   Updated: 2026/01/09 22:15:18 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirs = NULL;
	cmd->limits = NULL;
	cmd->heredoc_fd = -1;
	cmd->next = NULL;
	return (cmd);
}

void	cmd_add_back(t_cmd **cmds, t_cmd *new_node)
{
	t_cmd	*tmp;

	if (!cmds || !new_node)
		return ;
	if (!*cmds)
	{
		*cmds = new_node;
		return ;
	}
	tmp = *cmds;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_node;
}

static int	argv_len(char **argv)
{
	int	i;

	i = 0;
	if (!argv)
		return (0);
	while (argv[i])
		i++;
	return (i);
}

void	cmd_add_arg(t_cmd *cmd, char *arg)
{
	int		len;
	char	**new_argv;
	int		i;

	len = argv_len(cmd->args);
	new_argv = malloc(sizeof(char *) * (len + 2));
	if (!new_argv)
		return ;
	i = 0;
	while (i < len)
	{
		new_argv[i] = cmd->args[i];
		i++;
	}
	new_argv[len] = ft_strdup(arg);
	new_argv[len + 1] = NULL;
	free(cmd->args);
	cmd->args = new_argv;
}

void	redir_add_back(t_redir **list, t_redir *new_node)
{
	t_redir	*tmp;

	if (!list || !new_node)
		return ;
	if (!*list)
	{
		*list = new_node;
		return ;
	}
	tmp = *list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_node;
}
