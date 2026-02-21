/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 01:10:09 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:46:30 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isalpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (1);
	else
		return (0);
}
/*
#include <ctype.h>
#include <stdio.h>

int	main(void)
{
	int	c;

	c = 'a';
	printf ("meu: %d\n", ft_isalpha(c));
	printf ("original: %d\n\n", isalpha(c));
	c = '1';
	printf ("meu: %d\n", ft_isalpha(c));
	printf ("original: %d\n\n", isalpha(c));
	c = 'A';
	printf ("meu: %d\n", ft_isalpha(c));
	printf ("original: %d\n", isalpha(c));
}*/
