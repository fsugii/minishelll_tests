/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 10:29:17 by fshiniti          #+#    #+#             */
/*   Updated: 2026/01/29 18:19:28 by abroslav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_pipe_syntax(t_token *current)
{
	if (!current->next || current->next->type == TK_PIPE)
	{
		ft_putendl_fd(
			"minishell: syntax error near unexpected token `|'", 2);
		return (2);
	}
	return (0);
}

static int	check_redir_syntax(t_token *current)
{
	if (!current->next || current->next->type != TK_WORD)
	{
		ft_putendl_fd(
			"minishell: syntax error near unexpected token `newline'", 2);
		return (2);
	}
	return (0);
}

static int	validate_syntax(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	if (!current)
		return (0);
	if (current->type == TK_PIPE)
	{
		ft_putendl_fd(
			"minishell: syntax error near unexpected token `|'", 2);
		return (2);
	}
	while (current)
	{
		if (current->type == TK_PIPE && check_pipe_syntax(current))
			return (2);
		if ((current->type >= TK_REDIR_IN
				&& current->type <= TK_HEREDOC)
			&& check_redir_syntax(current))
			return (2);
		current = current->next;
	}
	return (0);
}

static void	process_token(t_cmd **cmds, t_cmd **cur, t_token **tokens)
{
	if ((*tokens)->type == TK_WORD)
	{
		if ((*tokens)->value[0] != '\0' || (*tokens)->no_expand
			|| (*tokens)->in_dquotes)
			cmd_add_arg(*cur, (*tokens)->value);
	}
	else if ((*tokens)->type == TK_PIPE)
	{
		*cur = new_cmd();
		cmd_add_back(cmds, *cur);
	}
	else if ((*tokens)->type == TK_REDIR_IN
		|| (*tokens)->type == TK_HEREDOC)
		parse_redir_in(*cur, tokens);
	else if ((*tokens)->type == TK_REDIR_OUT
		|| (*tokens)->type == TK_APPEND)
		parse_redir_out(*cur, tokens);
}

t_cmd	*parser(t_token *tokens, t_shell *shell)
{
	t_cmd	*cmds;
	t_cmd	*current_cmd;

	if (!tokens)
		return (NULL);
	if (validate_syntax(tokens) != 0)
	{
		shell->exit_code = 2;
		return (NULL);
	}
	cmds = NULL;
	current_cmd = new_cmd();
	cmd_add_back(&cmds, current_cmd);
	while (tokens)
	{
		process_token(&cmds, &current_cmd, &tokens);
		tokens = tokens->next;
	}
	return (cmds);
}
