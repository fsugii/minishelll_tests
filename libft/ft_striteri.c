/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 22:53:27 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:34:04 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	int	i;

	i = 0;
	while (s[i])
	{
		f(i, &s[i]);
		i++;
	}
}
/*
static void	to_lower(unsigned int i, char *s)
{
	(void) i;
	if (*s >= 'A' && *s <= 'Z')
		*s = *s + 32;
}
int main(void)
{
	char string[] = "TO_LOWERCASE";
	ft_striteri(string, to_lower);
	printf("Final content of the string: %s\n", string);
	return (0);
}*/
