SHELL = /bin/sh

CC      = clang
CFLAGS  = -g -Wall
SRC_DIR = src
SRC     = sha-3.c
OBJ_DIR = obj
OBJ     = $(SRC:.c=.o)
BIN_DIR = bin
BIN     = sha-3

.PHONY : all
all : $(BIN_DIR)/$(BIN)

$(addprefix $(BIN_DIR)/,$(BIN)) : $(addprefix $(OBJ_DIR)/,$(OBJ)) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(addprefix $(OBJ_DIR)/,%.o) : $(addprefix $(SRC_DIR)/,%.c) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BIN_DIR):
	mkdir $@

$(OBJ_DIR):
	mkdir $@

.PHONY : clean
clean :
	rm -f $(addprefix $(OBJ_DIR)/,$(OBJ)) $(BIN_DIR)/$(BIN)
