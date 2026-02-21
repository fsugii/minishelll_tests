/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 11:49:28 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/14 12:42:17 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_toupper(int c)
{
	if (c > 96 && c < 123)
		c = c - 32;
	return (c);
}
/*
int main(void)
{
	char str[] = "aBcDe@";
	for (int i = 0; str[i] != '\0'; i++)
	{
		str[i] = ft_toupper(str[i]);
	}
	printf ("%s\n", str);
}*/
