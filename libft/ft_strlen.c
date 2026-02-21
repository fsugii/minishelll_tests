/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 00:50:58 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/27 23:06:50 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0')
		i++;
	return (i);
}
/*
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
	char string[] = "Testing Characters Count!";
	printf ("Characters Total: %zu\n", ft_strlen(string));
	printf ("Characters Total Original Function: %zu\n", strlen(string));
}*/
