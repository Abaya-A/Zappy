#
# Project  -  Zappy
# Date     -  May 27, 2026
# 
# Copyright (c) 2026 Léo Lacordaire
# 

NAME        := zappy_server
CARGO       := cargo
CARGO_FLAGS ?=
BUILD_BIN   := target/debug/$(NAME)
LOCAL_BIN   := $(NAME)

.PHONY: all build run clean fclean re

all: build

build:
	$(CARGO) build $(CARGO_FLAGS)
	cp $(BUILD_BIN) $(LOCAL_BIN)

run:
	./$(LOCAL_BIN)

clean:
	$(CARGO) clean
	rm -f $(LOCAL_BIN)

fclean: clean

re: fclean all

