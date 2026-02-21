/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 18:00:00 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/20 18:00:00 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	heredoc_has_quotes(char *delimiter)
{
	int	i;

	i = 0;
	while (delimiter[i])
	{
		if (delimiter[i] == '\'' || delimiter[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

char	*heredoc_remove_quotes(char *delimiter)
{
	int		len;
	char	*clean;

	len = ft_strlen(delimiter);
	if (len >= 2 && ((delimiter[0] == '\'' && delimiter[len - 1] == '\'')
			|| (delimiter[0] == '"' && delimiter[len - 1] == '"')))
	{
		clean = ft_substr(delimiter, 1, len - 2);
		return (clean);
	}
	return (ft_strdup(delimiter));
}

char	*heredoc_gen_temp_filename(void)
{
	static int	counter = 0;
	char		*num;
	char		*filename;

	num = ft_itoa(counter++);
	filename = ft_strjoin(".heredoc_tmp_", num);
	free(num);
	return (filename);
}

void	heredoc_eof_warning(char *delimiter)
{
	ft_putstr_fd("minishell: warning: here-document delimited ", 2);
	ft_putstr_fd("by end-of-file (wanted '", 2);
	ft_putstr_fd(delimiter, 2);
	ft_putstr_fd("')\n", 2);
}

char	*heredoc_read_line(void)
{
	char	buffer[1024];
	int		i;
	int		ret;

	i = 0;
	while (i < 1023)
	{
		ret = read(STDIN_FILENO, &buffer[i], 1);
		if (ret < 0)
			return (NULL);
		if (ret == 0 || buffer[i] == '\n')
		{
			buffer[i] = '\0';
			if (ret == 0 && i == 0)
				return (NULL);
			return (ft_strdup(buffer));
		}
		i++;
	}
	buffer[i] = '\0';
	return (ft_strdup(buffer));
}
