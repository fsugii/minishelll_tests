# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/22 19:38:02 by fshiniti          #+#    #+#              #
#    Updated: 2025/04/22 19:38:15 by fshiniti         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME= libft.a

SRCS= ft_isalpha.c ft_isdigit.c ft_isalnum.c ft_isascii.c ft_isprint.c ft_strlen.c ft_memset.c ft_putstr_fd.c ft_bzero.c ft_memcpy.c ft_memmove.c ft_strlcpy.c ft_strlcat.c ft_toupper.c ft_tolower.c ft_strchr.c ft_strrchr.c ft_strncmp.c ft_memchr.c ft_memcmp.c ft_strnstr.c ft_atoi.c ft_calloc.c ft_strdup.c ft_substr.c ft_strjoin.c ft_strtrim.c ft_split.c ft_itoa.c ft_strmapi.c ft_striteri.c ft_putchar_fd.c ft_putendl_fd.c ft_putnbr_fd.c

OBJS= $(SRCS:.c=.o)

BONUS_SRCS= ft_lstnew_bonus.c ft_lstadd_front_bonus.c ft_lstsize_bonus.c ft_lstlast_bonus.c ft_lstadd_back_bonus.c ft_lstdelone_bonus.c ft_lstclear_bonus.c ft_lstiter_bonus.c ft_lstmap_bonus.c

BONUS_OBJS= $(BONUS_SRCS:.c=.o)

CC= cc -Wall -Wextra -Werror

# Marcar quando os bônus foram adicionados
BONUS_FLAG= .bonus_added

$(NAME): $(OBJS)
	@ar -rcs $(NAME) $(OBJS)

bonus: $(BONUS_FLAG)

$(BONUS_FLAG): $(NAME) $(BONUS_OBJS)
	@ar -rcs $(NAME) $(BONUS_OBJS)
	@touch $(BONUS_FLAG)

all: $(NAME)

%.o: %.c
	@$(CC) -c $< -o $@ -I .

clean:
	@rm -rf $(OBJS) $(BONUS_OBJS) $(BONUS_FLAG)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
