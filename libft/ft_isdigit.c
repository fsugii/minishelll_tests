/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 02:33:30 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:46:35 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isdigit(int n)
{
	if (n >= '0' && n <= '9')
		return (1);
	else
		return (0);
}

/*
#include <stdio.h>
#include <ctype.h>

int	main(void)
{
	int	n;

	n = '5';
	printf ("Meu: %d\n", ft_isdigit(n));
	printf ("Sistema: %d\n\n", isdigit(n));
	n = 'b';
	printf ("Meu: %d\n", ft_isdigit(n));
	printf ("Sistema: %d\n\n", isdigit(n));
}*/
