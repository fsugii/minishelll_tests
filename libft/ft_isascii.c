/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 18:55:33 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/27 23:36:56 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isascii(int c)
{
	if ((c >= 0 && c < 128))
		return (1);
	else
		return (0);
}
/*
#include <stdio.h>
#include <ctype.h>

int main(void)
{
	printf("%d\n", ft_isascii(127));
}*/
