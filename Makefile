# Compilateurs / lieur
CC  := gcc
LD  := ld

# Flags
MAIN_COMP_ARGS := -c -fno-stack-protector -MMD -MP -g -O0
INCL_COMP_ARGS := $(MAIN_COMP_ARGS) -I include
LINK_ARGS      := -e _start

# Dossiers
BUILD_DIR      := build
BUILD_SRC_DIR  := $(BUILD_DIR)/src
BUILD_LIBC_DIR := $(BUILD_SRC_DIR)/libc

# Sources & objets
MAIN_SRCS := $(wildcard src/*.c)
LIBC_SRCS := $(wildcard src/libc/*.c)

MAIN_OBJS := $(patsubst src/%.c,$(BUILD_SRC_DIR)/%.o,$(MAIN_SRCS))
LIBC_OBJS := $(patsubst src/libc/%.c,$(BUILD_LIBC_DIR)/%.o,$(LIBC_SRCS))

CRT0_OBJ  := $(BUILD_DIR)/crt0.o
OBJS      := $(CRT0_OBJ) $(MAIN_OBJS) $(LIBC_OBJS)

# Cible par défaut
all: prog
build: prog

prog: $(OBJS)
	$(LD) $(LINK_ARGS) -o $@ $(CRT0_OBJ) $(MAIN_OBJS) $(LIBC_OBJS)

# ---- Assemblage ----
$(CRT0_OBJ): crt0.asm | $(BUILD_DIR)
	nasm -f elf64 $< -o $@

# ---- Compilation C ----
# Règle pour src/*.c -> build/src/*.o (main, sans -I include)
$(BUILD_SRC_DIR)/%.o: src/%.c | $(BUILD_SRC_DIR)
	$(CC) $(MAIN_COMP_ARGS) -c $< -o $@

# Règle pour src/libc/*.c -> build/src/libc/*.o (libc, avec -I include)
$(BUILD_LIBC_DIR)/%.o: src/libc/%.c | $(BUILD_LIBC_DIR)
	$(CC) $(INCL_COMP_ARGS) -c $< -o $@

# Création des dossiers si besoin
$(BUILD_DIR) $(BUILD_SRC_DIR) $(BUILD_LIBC_DIR):
	mkdir -p $@

# Nettoyage
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
	rm -f $(BUILD_SRC_DIR)/*.o $(BUILD_SRC_DIR)/*.d
	rm -f $(BUILD_LIBC_DIR)/*.o $(BUILD_LIBC_DIR)/*.d

fclean: clean
	rm -f prog

re: fclean all

.PHONY: all build clean fclean re

# Inclusion des fichiers de dépendances générés par -MMD -MP
-include $(OBJS:.o=.d)

