/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 15:32:30 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/13 16:15:36 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size != 0)
	{
		while (src[i] != '\0' && i < size -1)
		{
			dst[i] = src[i];
			i++;
		}
		dst[i] = '\0';
	}
	return (ft_strlen(src));
}
/*
int main(void)
{
	char dst[10];
	char src[] = "A wine and Pasta";
	ft_strlcpy(dst, src, sizeof(dst));
	printf ("Now, we have the following dest: %s\n", dst);
	printf ("Size of dest: %zu\n", ft_strlen(dst));
	printf ("Size of src: %zu\n", ft_strlen(src));
}*/
