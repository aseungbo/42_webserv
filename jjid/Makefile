
NAME := jjid

CC := c++
CFLAGS := -Wall -Wextra -Werror
CDEBUG = -fsanitize=address -g -std=c++98

SRCS := Location.cpp \
		parseConf.cpp \
		Request.cpp \
		Response.cpp \
		Server.cpp \
		uniHeader.cpp \
		WebServer.cpp \
		parseConf.cpp \
		parseUtil.cpp \
		Client.cpp


OBJ_DIR := ./objs/
OBJ_NAME := $(SRCS:.cpp=.o)
OBJ := $(addprefix $(OBJ_DIR), $(OBJ_NAME))

.PHONY : all clean fclean re

$(NAME) : $(OBJ)
	@$(CC) $(CFLAGS) $(CDEBUG) -o $@ $^

$(OBJ_DIR)%.o : %.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

all : $(NAME)

clean :
		@rm -rf $(OBJ_DIR)
		@echo "\x1b[31mCLEAN_$(NAME)\x1b[0m"

fclean : clean
		@rm -f $(NAME)
		@echo "\x1b[31;101mFCLEAN_$(NAME)\x1b[0m"

re : fclean all