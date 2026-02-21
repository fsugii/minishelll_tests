/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 12:46:07 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/14 13:01:30 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_tolower(int c)
{
	if (c > 64 && c < 91)
		c = c + 32;
	return (c);
}
/*
int main(void)
{
	char string[] = "TestE#!";
	for (int i = 0; string[i] != '\0'; i++)
		string[i] = ft_tolower(string[i]);
	printf ("Result: %s\n", string);
}*/
