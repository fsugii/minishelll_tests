/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 15:50:45 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/27 23:24:01 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalnum(int c)
{
	if (ft_isalpha(c) || ft_isdigit(c))
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

	c = '/';
	printf ("Resultado: %d\n", ft_isalnum(c));
	c = '1';
	printf ("Resultado: %d\n", isalnum(c));
	c = 'a';
	printf ("Resultado: %d\n", ft_isalnum(c));
	c = 'B';
	printf ("Resultado: %d\n", isalnum(c));
}*/
