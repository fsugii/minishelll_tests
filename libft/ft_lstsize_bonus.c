/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 01:05:52 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/23 20:44:59 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int	count;

	count = 0;
	while (lst)
	{
		count++;
		lst = lst->next;
	}
	return (count);
}
/*
int main(void)
{
	t_list	*temp;
	t_list	*head;
	t_list	*a;
	t_list	*b;

	a = malloc(sizeof(t_list));
	b = malloc(sizeof(t_list));

	head = a;
	a->next = b;
	b->next = NULL;

	printf ("%d\n", ft_lstsize(head));
	while (head)
	{
		temp = head;
		head = head->next;
		free(temp);
	}
	return (0);
}*/
