/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 22:05:29 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:46:38 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isprint(int c)
{
	return (c >= 32 && c <= 126);
}
/*
#include <stdio.h>
#include <ctype.h>

int	main(void)
{
	int	c;

	c = 'A';
	printf ("Resultado: %d\n", ft_isprint(c));
	c = 'A';
	printf ("Resultado: %d\n", isprint(c));
	c = '	';
	printf ("Resultado: %d\n", ft_isprint(c));
	c = '	';
	printf ("Resultado: %d\n", isprint(c));
}*/
