/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abroslav <abroslav@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 23:00:00 by fshiniti          #+#    #+#             */
/*   Updated: 2026/02/20 23:00:00 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	parse_sign_and_skip(const char *str, int *i, int *sign)
{
	*sign = 1;
	while (ft_isspace(str[*i]))
		(*i)++;
	if (str[*i] == '-' || str[*i] == '+')
	{
		if (str[*i] == '-')
			*sign = -1;
		(*i)++;
	}
	return (!(str[*i] >= '0' && str[*i] <= '9'));
}

static int	overflow_check(unsigned long long n, char c, int sign)
{
	unsigned long long	max_div;
	unsigned long long	max_mod;

	max_div = (unsigned long long)LLONG_MAX / 10;
	max_mod = (unsigned long long)LLONG_MAX % 10 + (sign == -1);
	if (n > max_div || (n == max_div
			&& (unsigned long long)(c - '0') > max_mod))
		return (1);
	return (0);
}

int	ft_atoll_overflow(const char *str, long long *res)
{
	unsigned long long	nmbr;
	int					sign;
	int					i;

	nmbr = 0;
	i = 0;
	if (parse_sign_and_skip(str, &i, &sign))
		return (1);
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (overflow_check(nmbr, str[i], sign))
			return (1);
		nmbr = nmbr * 10 + (str[i++] - '0');
	}
	*res = nmbr * sign;
	return (str[i] != '\0');
}

static char	*append_char(char *line, char c)
{
	size_t	len;
	char	*new_line;

	if (!line)
		len = 0;
	else
		len = ft_strlen(line);
	new_line = malloc(len + 2);
	if (!new_line)
		return (free(line), NULL);
	if (line)
		ft_memcpy(new_line, line, len);
	new_line[len] = c;
	new_line[len + 1] = '\0';
	free(line);
	return (new_line);
}

char	*read_line(void)
{
	char	c;
	char	*line;
	int		ret;

	line = NULL;
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0)
		{
			if (!line)
				return (NULL);
			return (line);
		}
		if (c == '\n')
			return (line);
		line = append_char(line, c);
		if (!line)
			return (NULL);
	}
}
