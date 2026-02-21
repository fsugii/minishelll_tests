/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 16:01:21 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:46:48 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_front(t_list **lst, t_list *new)
{
	if (!lst || !new)
		return ;
	new->next = *lst;
	*lst = new;
}
/*
int main(void)
{
	t_list *node1 = ft_lstnew("First");
	t_list *node2 = ft_lstnew("Second");
	t_list *node3 = ft_lstnew("Third");
	t_list *node4 = ft_lstnew("FirstOfAll");
	t_list *tmp;

	node1->next = node2;
	node2->next = node3;
	node3->next = NULL;

	ft_lstadd_front(&node1, node4);

	printf("New First Content: %s\n", (char *)node1->content);

	while (node1)
{
	tmp = node1->next;
	free(node1);
	node1 = tmp;
}
	return (0);
}*/
