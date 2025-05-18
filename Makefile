NAME    = ft_otp
SRCS    = main.c ft_otp.c tools.c
HEADERS = ft_otp.h
OBJ_DIR = obj
OBJS    = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -lssl -lcrypto

##############################################

ORANGE  = \033[0;33m
GREEN   = \033[1;32m
RED     = \033[0;31m
RESET   = \033[0m

##############################################

LIB_INSTALLED_FLAG = .lib_installed

all: lib $(NAME)

lib:
	@if [ ! -f $(LIB_INSTALLED_FLAG) ]; then \
		echo "🗃 Installing openssl, oathtool and libqrencode...$(ORANGE)"; \
		sudo apt-get install -y libssl-dev oathtool libqrencode-dev; \
		touch $(LIB_INSTALLED_FLAG); \
		echo "$(RESET)✅ openssl, oathtool and libqrencode installed$(ORANGE)"; \
	else \
		echo "✅ openssl, oathtool and libqrencode already installed$(ORANGE)"; \
	fi

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(RESET)✅ Compiled$(ORANGE)"

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(CFLAGS)
	@echo "$(RESET)✅ Linked$(ORANGE)"
	@echo "$(RED)"
	@cat ft_otp.txt
	@echo "$(GREEN)\n---> Ready to launch! <---$(RESET)\nUsage: ./$(NAME) [-g <filename>] or [-k ft_otp.key]\n$(ORANGE)"

seed:
	@head -c 32 /dev/urandom | xxd -p > key.txt

clean:
	@echo "🗑 Cleaning$(ORANGE)"
	rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(LIB_INSTALLED_FLAG)
	rm -rf */ *.key *.txt
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re lib

