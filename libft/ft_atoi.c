/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 08:35:31 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:46:12 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_atoi(const char *nptr)
{
	int	signal;
	int	result;

	signal = 1;
	result = 0;
	while ((*nptr >= 9 && *nptr <= 13) || *nptr == 32)
		nptr++;
	if (*nptr == '+')
	{
		nptr++;
	}
	else if (*nptr == '-')
	{
		signal = -signal;
		nptr++;
	}
	while (*nptr >= 48 && *nptr <= 57)
	{
		result = (result * 10) + (*nptr - '0');
		nptr++;
	}
	return (result * signal);
}
/*
int	main(int ac, char **av)
{
	if (ac == 2)
	{
		printf("My atoi: %d\n", ft_atoi(av[1]));
		printf("Real atoi: %d\n", atoi(av[1]));
	}
}*/
