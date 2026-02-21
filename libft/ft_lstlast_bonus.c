/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 05:02:29 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:47:01 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
	{
		lst = lst->next;
	}
	return (lst);
}
/*
int main(void)
{
	t_list *node1 = ft_lstnew("First");
	t_list *node2 = ft_lstnew("Second");
	t_list *node3 = ft_lstnew("Third");

	node1->next = node2;
	node2->next = node3;
	node3->next = NULL;

	t_list *last = ft_lstlast(node1);

	if (last)
		printf("Last item content: %s\n", (char *)last->content);
	else
		printf("Empty list\n");

	free(node1);
	free(node2);
	free(node3);

	return (0);
}*/
