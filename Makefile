#
# Project  -  Zappy
# Date     -  May 27, 2026
# 
# Copyright (c) 2026 Abaya Team
# 

SERVER_DIR  := server
SERVER_BIN  := $(SERVER_DIR)/zappy_server
ROOT_BIN    := zappy_server

.PHONY: all server clean fclean re

all: server $(ROOT_BIN)

server:
	$(MAKE) -C $(SERVER_DIR)

$(ROOT_BIN): server
	cp $(SERVER_BIN) $(ROOT_BIN)
	chmod +x $(ROOT_BIN)

clean:
	$(MAKE) -C $(SERVER_DIR) clean
	rm -f $(ROOT_BIN)

fclean:
	$(MAKE) -C $(SERVER_DIR) fclean
	rm -f $(ROOT_BIN)

re: fclean all

