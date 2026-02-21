/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:01:46 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:46:59 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	if (!lst || !f)
		return ;
	while (lst)
	{
		f(lst->content);
		lst = lst->next;
	}
}
/*
// Função auxiliar para aplicar em cada elemento
void print_content(void *content)
{
    if (content)
        printf("%s\n", (char *)content);
}

int main()
{
    // Criando uma lista encadeada simples
    t_list *node1 = ft_lstnew("Primeiro");
    t_list *node2 = ft_lstnew("Segundo");
    t_list *node3 = ft_lstnew("Terceiro");

    // Ligando os nós
    node1->next = node2;
    node2->next = node3;

    // Iterando e aplicando a função print_content
    printf("Conteúdo da lista:\n");
    ft_lstiter(node1, print_content);

    // Liberando memória alocada
    free(node1);
    free(node2);
    free(node3);

    return 0;
}*/
