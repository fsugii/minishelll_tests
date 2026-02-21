/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 17:13:05 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:36:34 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	unsigned int	i;
	char			*result;

	i = 0;
	if (!s || !f)
		return (NULL);
	result = (char *)malloc(sizeof(char) * (ft_strlen(s)) + 1);
	if (result == NULL)
		return (NULL);
	while (s[i] != '\0')
	{
		result[i] = f(i, s[i]);
		i++;
	}
	result[i] = '\0';
	return (result);
}
/*
static char	to_upper(unsigned int i, char str);

int main(void)
{
	char *str = "to_uppercase";
	char *result = ft_strmapi(str, to_upper);
	printf("Final Result: %s\n", result);
	return (0);
}

char	to_upper(unsigned int i, char str)
{
	if (str >= 'a' && str <= 'z')
		return (str - 32);
	else
		return (str);
}*/
