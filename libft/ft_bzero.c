/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 21:04:37 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:46:19 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	ft_memset(s, '\0', n);
}
/*
int	main(void)
{
	char buffer[10] = "abcdefghij";
	for (int i = 0; i < 10; i++)
		printf("%c ", buffer[i]);
	printf ("\n");
	ft_bzero(buffer, 5);
	for (int i = 0; i < 10; i++)
		printf("%d ", buffer[i]);
}*/
