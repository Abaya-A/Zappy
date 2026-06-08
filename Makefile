#
# Project  -  Zappy
# Date     -  May 27, 2026
#
# Copyright (c) 2026 Abaya Team
#

SERVER_DIR      := server
SERVER_BIN      := $(SERVER_DIR)/zappy_server
SERVER_ROOT     := zappy_server

GUI_DIR         := gui
GUI_NAME        := zappy_gui
GUI_BUILD_DIR   := $(GUI_DIR)/build
GUI_BUILD_BIN   := $(GUI_BUILD_DIR)/$(GUI_NAME)

.PHONY: all server zappy_server zappy_gui clean fclean re clean_gui fclean_gui

all: zappy_server zappy_gui

server: zappy_server

zappy_server:
	$(MAKE) -C $(SERVER_DIR)
	cp $(SERVER_BIN) $(SERVER_ROOT)
	chmod +x $(SERVER_ROOT)

zappy_gui:
	cmake -S $(GUI_DIR) -B $(GUI_BUILD_DIR)
	cmake --build $(GUI_BUILD_DIR)
	cp $(GUI_BUILD_BIN) ./$(GUI_NAME)
	chmod +x ./$(GUI_NAME)

clean: clean_tests clean_gui
	$(MAKE) -C $(SERVER_DIR) clean
	rm -f $(SERVER_ROOT)

clean_gui:
	cmake --build $(GUI_BUILD_DIR) --target clean 2>/dev/null || true

fclean: fclean_tests
	$(MAKE) -C $(SERVER_DIR) fclean
	rm -f $(SERVER_ROOT)
	rm -f $(GUI_NAME)
	rm -rf $(GUI_BUILD_DIR)

fclean_gui:
	rm -f $(GUI_NAME)
	rm -rf $(GUI_BUILD_DIR)

re: fclean all

include tests/Makefile