/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 20:50:58 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/27 23:08:55 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_count_letters(long nb)
{
	int	len;

	len = 0;
	if (nb <= 0)
	{
		len++;
	}
	while (nb != 0)
	{
		nb = nb / 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	int			len;
	long		nbr;
	char		*result;

	nbr = n;
	if (nbr == 0)
		return (ft_strdup("0"));
	len = ft_count_letters(nbr);
	result = malloc((len + 1) * sizeof (char));
	if (!result)
		return (NULL);
	result[len--] = '\0';
	if (nbr < 0)
	{
		result[0] = '-';
		nbr = -nbr;
	}
	while (nbr != 0)
	{
		result[len--] = ((nbr % 10) + 48);
		nbr = nbr / 10;
	}
	return (result);
}
/*
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

int	main(void)
{
	printf("string :%s\n", ft_itoa(INT32_MIN));
	printf("string :%s\n", ft_itoa(INT32_MAX));
	printf("string :%s\n", ft_itoa(-42));
	printf("string :%s\n", ft_itoa(2));
	printf("string :%s\n", ft_itoa(0));
	printf("string :%s\n", ft_itoa(42));
	return (0);
}*/
