# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dlacuey <dlacuey@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/04 19:44:50 by dlacuey           #+#    #+#              #
#    Updated: 2023/08/08 17:16:08 by dlacuey          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = gcc

CFLAGS = -Wall -Wextra -Werror

OBJS = main.o

NAME = pipex

LIBFT = ./libft/libft.a

all: $(NAME)

$(NAME): $(OBJS)
	make -C ./libft
	$(CC) -o $(NAME) $(OBJS) $(LIBFT)

clean:
	make clean -C ./libft
	$(RM) $(OBJS) $(OBJS_BONUS)

fclean: clean
	make fclean -C ./libft
	$(RM) $(NAME) $(MINILIBX)

re: fclean all

.PHONY: all clean fclean re bonus
