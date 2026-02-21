/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 02:14:07 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:46:55 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstdelone(t_list *lst, void (*del)(void *))
{
	if (!lst || !del)
		return ;
	del(lst->content);
	free(lst);
}
/*
void	print_list(t_list *lst)
{
	while (lst)
	{
		printf ("%s ->", (char *)lst->content);
		lst = lst->next;
	}
	printf ("NULL\n");
}

int	main(void)
{
	t_list *node1 = ft_lstnew(strdup("Node 1"));
	t_list *node2 = ft_lstnew(strdup("Node 2"));
	t_list *node3 = ft_lstnew(strdup("Node 3"));
	t_list *node4 = ft_lstnew(strdup("Node 4"));

	node1->next = node2;
	node2->next = node3;
	node3->next = node4;
	node4->next = NULL;

	printf("Lista original:\n");
	t_list *temp = node1;
	while (temp)
	{
		printf("%s -> ", (char *)temp->content);
		temp = temp->next;
	}
	printf("NULL\n");

	node1->next = node3;
	ft_lstdelone(node2, free);

	printf("\nLista após remover o segundo nó:\n");
	temp = node1;
	while (temp)
	{
		printf("%s -> ", (char *)temp->content);
		temp = temp->next;
	}
	printf("NULL\n");

	ft_lstdelone(node1, free);
	ft_lstdelone(node3, free);
	ft_lstdelone(node4, free);

	return (0);
}*/
