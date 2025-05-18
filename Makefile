NAME    = ft_otp
SRCS    = main.c ft_otp.c
HEADERS = ft_otp.h
OBJ_DIR = obj
OBJS    = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

CC      = gcc
CFLAGS  = -Wall -Wextra -Werror 
XFLAGS	= -lssl -lcrypto -lqrencode

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
		echo "🗃  Installing openssl, oathtool and libqrencode...$(ORANGE)"; \
		sudo apt-get install -y libssl-dev oathtool libqrencode-dev; \
		touch $(LIB_INSTALLED_FLAG); \
		echo "$(RESET)✅ openssl, oathtool and libqrencode installed$(ORANGE)"; \
	else \
		echo "✅ openssl, oathtool and libqrencode already installed$(ORANGE)"; \
	fi

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(XFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@echo "$(RESET)✅ Compiled$(ORANGE)"
	$(CC) $(OBJS) -o $(NAME) $(CFLAGS) $(XFLAGS)
	@echo "$(RESET)✅ Linked$(ORANGE)"
	@echo "$(RED)"
	@echo "$(GREEN)---> Ready to launch! <---$(RESET)\nUsage: ./$(NAME) [-g <filename>] or [-k ft_otp.key]\n$(ORANGE)"

seed:
	rm -rf key.txt
	head -c 32 /dev/urandom | xxd -p -c 64 > key.txt
	@echo 
	@cat key.txt

clean:
	@echo "🗑 Cleaning$(ORANGE)"
	rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(LIB_INSTALLED_FLAG)
	rm -rf */ *.key *.txt
	rm -f $(NAME)

.PHONY: all clean fclean re lib

