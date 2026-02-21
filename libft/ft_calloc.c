/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:46:03 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/27 23:23:00 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	char	*ptr;

	if (size && nmemb > SIZE_MAX / size)
		return (NULL);
	ptr = malloc(nmemb * size);
	if (ptr == NULL)
		return (NULL);
	(ft_memset(ptr, 0, nmemb * size));
	return (ptr);
}
/*
int main() 
{
    int n = 5; 
    int *arr = (int *)ft_calloc(n, sizeof(int)); 

    if (arr == NULL)
    { 
        printf("Erro ao alocar memória\n");
        return 1;
    }
    for (int i = 0; i < n; i++) 
    {
        printf("arr[%d] = %d\n", i, arr[i]);
    }
    free(arr); 
    return 0;
}*/
