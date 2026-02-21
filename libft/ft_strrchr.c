/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 18:49:47 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/15 18:02:53 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *str, int c)
{
	int	i;

	i = ft_strlen(str);
	while (i >= 0)
	{
		if ((unsigned char)str[i] == (unsigned char)c)
			return ((char *)&str[i]);
		i--;
	}
	return (NULL);
}
/*
int main(void)
{
	printf ("%s\n", strrchr("Finding Reverse Character", 'R'));
	char string[] = "Finding Reverse Charactere";
	char *find = ft_strrchr(string, 'R');
	printf ("Resultado: %s\n", find);
}*/
