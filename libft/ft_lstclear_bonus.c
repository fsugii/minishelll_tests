/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 04:31:59 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 23:46:50 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*tmp;

	if (!lst || !del)
		return ;
	while (*lst)
	{
		tmp = (*lst)->next;
		ft_lstdelone(*lst, del);
		*lst = tmp;
	}
	free(*lst);
	*lst = NULL;
}
/*
void del(void *content)
{
    free(content);
}

int main()
{
    // Criando dois nós simples
    t_list *node1 = malloc(sizeof(t_list));
    t_list *node2 = malloc(sizeof(t_list));

    // Preenchendo os nós
    node1->content = malloc(10); // Por exemplo, alocando espaço 
				 // para um dado qualquer
    node1->next = node2;

    node2->content = malloc(10); // Outro espaço para dado
    node2->next = NULL;

    // Ponteiro para o início da lista
    t_list *lst = node1;

    // Chamando ft_lstclear para limpar a lista
    ft_lstclear(&lst, del);

    // Verificando se a lista foi limpa (lst deve ser NULL)
    if (lst == NULL)
        printf("Lista limpa com sucesso.\n");
    else
        printf("Erro ao limpar a lista.\n");

    return 0;
}*/
