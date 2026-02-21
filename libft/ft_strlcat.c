/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 17:27:01 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:34:35 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	j;
	size_t	ts;
	size_t	td;

	td = ft_strlen(dst);
	ts = ft_strlen(src);
	i = td;
	j = 0;
	if (size <= td)
		return (ts + size);
	while ((src[j] != '\0') & (i < size -1))
	{
		dst[i] = src[j];
		j++;
		i++;
	}
	dst[i] = '\0';
	return (ts + td);
}
/*

#include <bsd/string.h>

int	main(void)
{
	size_t r = 0;
	char dst[6] = "Hello ";
	char src[] = "for All!!!";
	char bkp[100];
	strcpy (bkp, dst);
	printf ("Retorno esperado: %zu\n", strlcat (dst, src, sizeof(dst)));
	strcpy (dst, bkp);
	r = ft_strlcat(dst, src, sizeof(dst));
	printf ("Conteudo no dst: %s\n", dst);
	printf ("Caracteres em dest: %zu\n", ft_strlen(dst));
	printf ("Total de caracteres: %zu\n", r);
}*/
