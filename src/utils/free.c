/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 18:37:44 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/16 16:39:37 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_all(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->s_tokens)
	{
		free_tokens(shell->s_tokens);
		shell->s_tokens = NULL;
	}
	if (shell->s_cmds)
	{
		free_cmds(shell->s_cmds);
		shell->s_cmds = NULL;
	}
	if (shell->env_vars)
	{
		free_env(shell->env_vars);
		shell->env_vars = NULL;
	}
	rl_clear_history();
}

void	free_tokens(t_token *list)
{
	t_token	*tmp;

	while (list)
	{
		tmp = list;
		list = list->next;
		free(tmp->value);
		free(tmp);
	}
}

/*free a list of redirecions*/
static void	free_redirs(t_redir *redir)
{
	t_redir	*next;

	while (redir)
	{
		next = redir->next;
		if (redir->target)
			free(redir->target);
		free(redir);
		redir = next;
	}
}

/*frees the command list and its nested args/redirs*/
void	free_cmds(t_cmd *cmds)
{
	t_cmd	*tmp;
	int		i;

	while (cmds)
	{
		tmp = cmds;
		cmds = cmds->next;
		if (tmp->args)
		{
			i = 0;
			while (tmp->args[i])
				free(tmp->args[i++]);
			free(tmp->args);
		}
		if (tmp->redirs)
			free_redirs(tmp->redirs);
		if (tmp->heredoc_fd >= 0)
			close(tmp->heredoc_fd);
		free(tmp);
	}
}

void	cleanup_exit_child(t_shell *shell, int exit_code)
{
	if (!shell)
		exit(exit_code);
	if (shell->s_tokens)
		free_tokens(shell->s_tokens);
	if (shell->s_cmds)
		free_cmds(shell->s_cmds);
	if (shell->env_vars)
		free_env(shell->env_vars);
	exit(exit_code);
}
