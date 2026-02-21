/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memove.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 15:50:56 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/11 17:46:50 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;
	size_t				i;

	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	i = 0;
	if (d == s || n == 0)
		return (dest);
	if (d < s)
	{
		while (i < n)
		{
			d[i] = s[i];
			i++;
		}
	}
	else
	{
		while (n--)
		{
			d[n] = s[n];
		}
	}
	return (dest);
}
/*
int main(void)
{
	char str1[100] = "123456789";
	char str2[100] = "123456789";

	printf ("Antes: %s\n", str1);
	
	ft_memmove(str1 + 2, str1, 5);
	printf ("Depois: %s\n", str1);

	memmove (str2 + 2, str2, 5);
	printf ("Com memmove original: %s\n", str2);
	return (0);
}*/
