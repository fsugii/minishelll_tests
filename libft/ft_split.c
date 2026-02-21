/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:21:30 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:44:15 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_countwords(const char *s, char c);

static void	ft_free(char **result);

static int	ft_search(const char *s, char c);

char	**ft_split(char const *s, char c)
{
	char		**result;
	int			i;
	int			w_len;

	if (!s)
		return (NULL);
	result = (char **)ft_calloc(ft_countwords(s, c) + 1, sizeof(char *));
	if (!result)
		return (NULL);
	i = 0;
	while (*s != '\0')
	{
		while (*s == c && *s != '\0')
			s++;
		if (*s != '\0')
		{
			w_len = ft_search(s, c);
			result[i] = ft_substr(s, 0, w_len);
			if (!result[i])
				return (ft_free(result), NULL);
			i++;
			s = s + w_len;
		}
	}
	return (result);
}

/*
int	main(void)
{
	char	**result;
	int		i;

	char	*str = "Testing word separation in phrases";
	result = ft_split(str, ' ');
	i = 0;
	while (result[i] != NULL)
	{
		printf("Result[%d] - %s\n", i, result[i]);
		i++;
	}
	ft_free(result);
}
*/

static int	ft_countwords(const char *s, char c)
{
	size_t	count;
	int		i;

	i = 0;
	count = 0;
	while (s[i] != '\0')
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
		else
			i++;
	}
	return (count);
}

static void	ft_free(char **result)
{
	int	i;

	i = 0;
	while (result[i] != NULL)
		free(result[i++]);
	free(result);
}

static int	ft_search(const char *s, char c)
{
	int	w_len;

	w_len = 0;
	if (*s != '\0')
	{
		if (!ft_strchr(s, c))
			w_len = ft_strlen(s);
		else
			w_len = ft_strchr(s, c) - s;
	}
	return (w_len);
}
