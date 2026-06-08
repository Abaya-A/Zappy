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
GUI_CXX         := g++
GUI_CXXFLAGS    := -Wall -Wextra -Werror -std=c++20 -I$(GUI_DIR)/include

GUI_SRC         := $(GUI_DIR)/src/main.cpp \
                   $(GUI_DIR)/src/client/GuiArgs.cpp \
                   $(GUI_DIR)/src/client/GuiClient.cpp \
                   $(GUI_DIR)/src/net/NetworkClient.cpp \
                   $(GUI_DIR)/src/protocol/ProtocolParser.cpp \
                   $(GUI_DIR)/src/protocol/ProtocolCommand.cpp \
                   $(GUI_DIR)/src/protocol/GuiProtocolDecoder.cpp \
                   $(GUI_DIR)/src/protocol/GuiEventApplier.cpp \
                   $(GUI_DIR)/src/state/Resource.cpp \
                   $(GUI_DIR)/src/state/Tile.cpp \
                   $(GUI_DIR)/src/state/Egg.cpp \
                   $(GUI_DIR)/src/state/Player.cpp \
                   $(GUI_DIR)/src/state/GameState.cpp

GUI_OBJ         := $(GUI_SRC:.cpp=.o)

.PHONY: all server zappy_gui clean fclean re

all: server $(SERVER_ROOT) zappy_gui

server:
	$(MAKE) -C $(SERVER_DIR)

$(SERVER_ROOT): server
	cp $(SERVER_BIN) $(SERVER_ROOT)
	chmod +x $(SERVER_ROOT)

zappy_gui: $(GUI_OBJ)
	$(GUI_CXX) $(GUI_CXXFLAGS) -o $(GUI_NAME) $(GUI_OBJ)

%.o: %.cpp
	$(GUI_CXX) $(GUI_CXXFLAGS) -c $< -o $@

clean: clean_tests
	$(MAKE) -C $(SERVER_DIR) clean
	rm -f $(SERVER_ROOT)
	rm -f $(GUI_OBJ)

fclean: fclean_tests
	$(MAKE) -C $(SERVER_DIR) fclean
	rm -f $(SERVER_ROOT)
	rm -f $(GUI_OBJ)
	rm -f $(GUI_NAME)

re: fclean all

include tests/Makefile	