/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 13:36:42 by fshiniti          #+#    #+#             */
/*   Updated: 2026/01/06 18:54:54 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_tokens(t_token *tokens)
{
	printf("\n=== TOKENS ===\n");
	while (tokens)
	{
		printf("type=%d, value=\"%s\"\n", tokens->type, tokens->value);
		if (tokens->no_expand)
			printf(" [NO_EXPAND]\n");
		tokens = tokens->next;
	}
	printf("\n");
}

static void	print_redir(t_redir *redir)
{
	while (redir)
	{
		if (redir->type == REDIR_IN)
			printf(" < %s\n", redir->target);
		else if (redir->type == REDIR_OUT)
			printf(" > %s\n", redir->target);
		else if (redir->type == REDIR_APPEND)
			printf(" >> %s\n", redir->target);
		else if (redir->type == REDIR_HEREDOC)
			printf(" << %s\n", redir->target);
		redir = redir->next;
	}
}

void	print_cmds(t_cmd *cmd)
{
	int		i;

	printf("=== COMANDOS ===\n");
	while (cmd)
	{
		printf("--- CMD ---\n");
		if (cmd->args)
		{
			i = 0;
			while (cmd->args[i])
			{
				printf(" argv[%d] = \"%s\"\n", i, cmd->args[i]);
				i++;
			}
		}
		print_redir(cmd->redirs);
		cmd = cmd->next;
	}
	printf("\n");
}
