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
                   $(GUI_DIR)/src/GuiArgs.cpp \
                   $(GUI_DIR)/src/NetworkClient.cpp \
                   $(GUI_DIR)/src/GuiClient.cpp \
                   $(GUI_DIR)/src/ProtocolParser.cpp \
                   $(GUI_DIR)/src/ProtocolCommand.cpp \
				   $(GUI_DIR)/src/Resource.cpp \

GUI_OBJ         := $(GUI_SRC:.cpp=.o)

TEST_PROTOCOL_NAME := test_protocol
TEST_PROTOCOL_SRC  := tests/gui/test_protocol.cpp \
                      $(GUI_DIR)/src/ProtocolCommand.cpp \
                      $(GUI_DIR)/src/ProtocolParser.cpp

.PHONY: all server zappy_gui tests_run clean fclean re

all: server $(SERVER_ROOT) zappy_gui

server:
	$(MAKE) -C $(SERVER_DIR)

$(SERVER_ROOT): server
	cp $(SERVER_BIN) $(SERVER_ROOT)
	chmod +x $(SERVER_ROOT)

zappy_gui: $(GUI_OBJ)
	$(GUI_CXX) $(GUI_CXXFLAGS) -o $(GUI_NAME) $(GUI_OBJ)

tests_run:
	$(GUI_CXX) $(GUI_CXXFLAGS) -o $(TEST_PROTOCOL_NAME) $(TEST_PROTOCOL_SRC)
	./$(TEST_PROTOCOL_NAME)

%.o: %.cpp
	$(GUI_CXX) $(GUI_CXXFLAGS) -c $< -o $@

clean:
	$(MAKE) -C $(SERVER_DIR) clean
	rm -f $(SERVER_ROOT)
	rm -f $(GUI_OBJ)

fclean:
	$(MAKE) -C $(SERVER_DIR) fclean
	rm -f $(SERVER_ROOT)
	rm -f $(GUI_OBJ)
	rm -f $(GUI_NAME)
	rm -f $(TEST_PROTOCOL_NAME)

re: fclean all