/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:27:10 by fshiniti          #+#    #+#             */
/*   Updated: 2025/04/26 07:33:33 by fshiniti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_list;
	t_list	*new_elem;
	void	*content;

	if (!lst || !f || !del)
		return (NULL);
	new_list = NULL;
	while (lst)
	{
		content = f(lst->content);
		new_elem = ft_lstnew(content);
		if (!new_elem)
		{
			del(content);
			ft_lstclear(&new_list, del);
			return (NULL);
		}
		ft_lstadd_back(&new_list, new_elem);
		lst = lst->next;
	}
	return (new_list);
}

/*
// Função de mapeamento: converte o conteúdo para maiúsculas
void *to_uppercase(void *content)
{
    char *str = (char *)content;
    char *new_str = strdup(str);
    if (new_str)
    {
        for (int i = 0; new_str[i]; i++)
            if (new_str[i] >= 'a' && new_str[i] <= 'z')
                new_str[i] -= 32; // Converte para maiúscula
    }
    return new_str;
}

// Função para liberar o conteúdo da lista
void delete_content(void *content)
{
    free(content);
}

int main(void)
{
    // Criando uma lista simples
    t_list *node1 = ft_lstnew(strdup("abc"));
    t_list *node2 = ft_lstnew(strdup("def"));
    t_list *node3 = ft_lstnew(strdup("ghi"));

    // Conectando os nós
    ft_lstadd_back(&node1, node2);
    ft_lstadd_back(&node1, node3);

    // Aplicando ft_lstmap
    t_list *new_list = ft_lstmap(node1, to_uppercase, delete_content);

    // Exibindo conteúdo da nova lista
    for (t_list *temp = new_list; temp; temp = temp->next)
        printf("%s\n", (char *)temp->content);

    // Limpando memória
    ft_lstclear(&node1, delete_content);
    ft_lstclear(&new_list, delete_content);

    return (0);
}*/
