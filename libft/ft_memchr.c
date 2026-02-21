/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 21:10:44 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/15 21:18:06 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*ptr;
	unsigned char		tc;
	size_t				i;

	ptr = (const unsigned char *)s;
	tc = (unsigned char)c;
	i = 0;
	while (i < n)
	{
		if (ptr[i] == tc)
			return ((void *)(ptr + i));
		i++;
	}
	return (NULL);
}
/*
int main(void)
{
	char string[] = "Looking for character";
	char look_for = 'z';
	size_t s = sizeof(string);
	char *r = ft_memchr(string, look_for, s);

	if (r != NULL)
		printf("Found '%c' in the %lu position", *r, r - string);
	else
		printf("Character '%c' not found in the %lu first bytes\n", look_for, s);
	return (0);
}*/
