/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 02:22:55 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/27 23:35:31 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*ft_newstring(const char *s1, size_t start, size_t len);

static int	ft_trim(const char *set, char c);

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t		i;
	size_t		e;	

	i = 0;
	e = ft_strlen(s1) - 1;
	if (!s1 || !set)
		return (NULL);
	if (ft_strlen(s1) == 0)
		return (ft_strdup(""));
	while (ft_trim(set, s1[i]))
		i++;
	while (ft_trim(set, s1[e]))
		e--;
	return (ft_newstring(s1, i, e - i + 1));
}
/*
int	main(void)
{
	char	*string = "abaaSelobaba";
	printf("Result: %s\n", ft_strtrim(string, "ab"));
}*/

static int	ft_trim(const char *set, char c)
{
	size_t	i;

	i = 0;
	while (set[i])
	{
		if (c == set[i])
			return (1);
		i++;
	}
	return (0);
}

static char	*ft_newstring(const char *s1, size_t start, size_t len)
{
	char	*trim;
	size_t	i;

	if (len <= 0 || start >= ft_strlen(s1))
		return (ft_strdup(""));
	trim = malloc(sizeof(char) * len + 1);
	if (trim == NULL)
		return (NULL);
	i = 0;
	while (i < len)
	{
		trim[i] = s1[start];
		i++;
		start++;
	}
	trim[i] = '\0';
	return (trim);
}
