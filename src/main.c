/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 21:49:22 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/19 00:50:49 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_input(t_shell *shell)
{
	char	*line;

	if (isatty(STDIN_FILENO))
		line = readline("minishell> ");
	else
		line = read_line();
	if (g_last_signal == SIGINT)
	{
		shell->exit_code = 130;
		g_last_signal = 0;
	}
	return (line);
}

static void	shell_loop(t_shell *shell)
{
	char	*line;

	while (1)
	{
		line = get_input(shell);
		if (!line)
		{
			if (isatty(STDIN_FILENO))
				printf("exit\n");
			break ;
		}
		if (*line)
			add_history(line);
		if (validate_line(line, shell))
			continue ;
		process_line(line, shell);
		free(line);
	}
}

static void	init_shell(t_shell *shell, char **envp)
{
	shell->env_vars = copy_env(envp);
	shell->exit_code = 0;
	shell->s_tokens = NULL;
	shell->s_cmds = NULL;
}

int	main(int argc, char **argv, char **envp)
{
	t_shell		shell;

	(void)argc;
	(void)argv;
	init_shell(&shell, envp);
	if (!shell.env_vars)
		return (1);
	shlvl_update(&shell.env_vars);
	setup_signals();
	shell_loop(&shell);
	free_env(shell.env_vars);
	rl_clear_history();
	return (0);
}
