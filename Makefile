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

AI_DIR 			:= ai
AI_NAME        	:= zappy_ai
AI_RUN			:= run_ai.sh
AI_BUILD  		:= $(AI_DIR)/$(AI_NAME)

.PHONY: all server zappy_server zappy_gui zappy_ai clean fclean re clean_gui fclean_gui clean_ai fclean_ai

all: zappy_server zappy_gui zappy_ai

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

zappy_ai:
	cp $(AI_BUILD) ./$(AI_NAME)
	cp $(AI_BUILD) ./$(AI_RUN)
	chmod +x ./$(AI_NAME)
	chmod +x ./$(AI_RUN)


clean: clean_tests clean_gui clean_ai
	$(MAKE) -C $(SERVER_DIR) clean
	rm -f $(SERVER_ROOT)

clean_gui:
	cmake --build $(GUI_BUILD_DIR) --target clean 2>/dev/null || true

clean_ai:
	find $(AI_DIR) -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	find $(AI_DIR) -name "*.pyc" -delete 2>/dev/null || true

fclean: fclean_tests fclean_ai
	$(MAKE) -C $(SERVER_DIR) fclean
	rm -f $(SERVER_ROOT)
	rm -f $(GUI_NAME)
	rm -rf $(GUI_BUILD_DIR)
	rm -f $(AI_NAME)
	rm -f $(AI_RUN)

fclean_gui:
	rm -f $(GUI_NAME)
	rm -rf $(GUI_BUILD_DIR)

fclean_ai: clean_ai

re: fclean all

include tests/Makefile