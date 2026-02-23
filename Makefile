# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fshiniti <fshiniti@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/22 18:35:54 by fshiniti          #+#    #+#              #
#    Updated: 2026/02/23 16:29:34 by fshiniti         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

#compiler
CC = cc

#flags
CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address
LDFLAGS = -lreadline -fsanitize=address

#directories
SRC_DIR = src
LIBFT_DIR = libft
INCLUDES_DIR = includes

#includes
INCLUDES = -I$(INCLUDES_DIR) -I$(LIBFT_DIR)

#libraries
LIBFT = $(LIBFT_DIR)/libft.a

#sources
SRCS    = $(SRC_DIR)/main.c \
          $(SRC_DIR)/utils/main_utils.c \
          $(SRC_DIR)/lexer/lexer.c \
          $(SRC_DIR)/lexer/lexer_utils.c \
          $(SRC_DIR)/lexer/lexer_word.c \
          $(SRC_DIR)/parser/parser.c \
          $(SRC_DIR)/parser/parser_utils.c \
          $(SRC_DIR)/parser/parser_redir.c \
          $(SRC_DIR)/utils/debug.c \
          $(SRC_DIR)/utils/free.c \
          $(SRC_DIR)/utils/utils.c \
          $(SRC_DIR)/utils/utils2.c \
          $(SRC_DIR)/expander/expander.c \
          $(SRC_DIR)/heredoc/heredoc.c \
          $(SRC_DIR)/heredoc/heredoc_utils.c \
          $(SRC_DIR)/env/env_init.c \
          $(SRC_DIR)/env/env_get.c \
          $(SRC_DIR)/env/env_modify.c \
          $(SRC_DIR)/exec/execute.c \
          $(SRC_DIR)/exec/child.c \
          $(SRC_DIR)/exec/redirs.c \
          $(SRC_DIR)/exec/path.c \
          $(SRC_DIR)/exec/exec_errors.c \
          $(SRC_DIR)/builtins/builtins_router.c \
          $(SRC_DIR)/builtins/builtins_info.c \
          $(SRC_DIR)/builtins/builtin_cd.c \
          $(SRC_DIR)/builtins/builtin_exit.c \
          $(SRC_DIR)/builtins/builtins_env.c \
          $(SRC_DIR)/builtins/export_print.c \
          $(SRC_DIR)/signals/signals.c

#objects
OBJS = $(SRCS:.c=.o)

# Cores
RED     = \033[0;31m
GREEN   = \033[0;32m
YELLOW  = \033[0;33m
BLUE    = \033[0;34m
PURPLE  = \033[0;35m
CYAN    = \033[0;36m
WHITE   = \033[0;37m
NC      = \033[0m # No Color

all: $(NAME) $(LIBFT) readline.supp

$(NAME): $(OBJS) $(LIBFT)
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(INCLUDES) $(LDFLAGS) -o $(NAME)
	@printf "$(GREEN)Minishell compilation complete!$(NC)\n"

$(LIBFT):
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR)
	@printf "$(BLUE)Libft compilation complete!$(NC)\n"

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@printf "$(YELLOW)Compiled: $<$(NC)\n"

clean:
	@rm -f $(OBJS)
	@rm -f readline.supp
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) clean
	@printf "$(RED)Clean complete!$(NC)\n"

fclean:
	@rm -f $(NAME) $(OBJS)
	@rm -f readline.supp
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) fclean
	@printf "$(RED)Full clean complete!$(NC)\n"

re: fclean all

readline.supp:
	@echo "{\n\
   leak readline\n\
   Memcheck:Leak\n\
   ...\n\
   fun:readline\n\
}\n\
{\n\
   leak add_history\n\
   Memcheck:Leak\n\
   ...\n\
   fun:add_history\n\
}" > readline.supp
	@printf "$(CYAN)readline.supp file created!$(NC)\n"

VALGRIND = valgrind
VG_FLAGS = --leak-check=full \
		   --show-leak-kinds=all \
		   --track-origins=yes \
		   --suppressions=readline.supp

valgrind: $(NAME)
	$(VALGRIND) $(VG_FLAGS) ./$(NAME)

valgrind_fd: $(NAME)
	$(VALGRIND) $(VG_FLAGS) --track-fds=yes ./$(NAME)

.PHONY: all clean fclean re valgrind valgrind_fd
