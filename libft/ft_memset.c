/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 16:09:08 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:31:40 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = (unsigned char *)b;
	i = 0;
	while (i < len)
	{
		ptr[i] = (unsigned char)c;
		i++;
	}
	return (b);
}
/*
#include <string.h>
#include <stdio.h>

int	main(void)
{
	char buffer[10] = "abcdefghij";
	for (int i = 0; i < 10; i++)
		printf("%c ", buffer[i]);
	printf ("\n");
	ft_memset(buffer, 0, sizeof(buffer));
	for (int i = 0; i < 10; i++)
		printf("%d ", buffer[i]);
}*/
