/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 21:43:38 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/23 20:44:40 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*item;

	item = malloc(sizeof(t_list));
	if (!item)
		return (NULL);
	item->content = content;
	item->next = NULL;
	return (item);
}
/*
int	main(void)
{
	t_list *teste;

	char str[] = "XPTO";
	teste = ft_lstnew(str);
	if (!teste)
		return (1);
	printf("%s\n", (char *)(teste->content));
	free(teste);
	return (0);
}*/
