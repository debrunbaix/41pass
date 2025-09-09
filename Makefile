# Compilateurs / lieur
CC  := gcc
LD  := ld

# Flags
MAIN_COMP_ARGS := -fno-stack-protector -MMD -MP -g -O0
INCL_COMP_ARGS := $(MAIN_COMP_ARGS) -I include
LINK_ARGS      := -e _start

# Dossiers
BUILD_DIR      := build
BUILD_SRC_DIR  := $(BUILD_DIR)/src
BUILD_LIBC_DIR := $(BUILD_SRC_DIR)/libc
BUILD_TEST_DIR := $(BUILD_DIR)/test/libc

# Sources & objets
MAIN_SRCS := $(wildcard src/*.c)
LIBC_SRCS := $(wildcard src/libc/*.c)

MAIN_OBJS := $(patsubst src/%.c,$(BUILD_SRC_DIR)/%.o,$(MAIN_SRCS))
LIBC_OBJS := $(patsubst src/libc/%.c,$(BUILD_LIBC_DIR)/%.o,$(LIBC_SRCS))

CRT0_OBJ  := $(BUILD_DIR)/crt0.o
OBJS      := $(CRT0_OBJ) $(MAIN_OBJS) $(LIBC_OBJS)

# Tests
TEST_LIBC_SRCS := $(patsubst %, test/libc/t_%.c, \
					getline open read write strcmp)

TEST_LIBC_OBJS  := $(patsubst %.c, $(BUILD_DIR)/%.o, $(TEST_LIBC_SRCS))
TEST_LIBC_BINS  := $(patsubst %.o, %, $(TEST_LIBC_OBJS))

# Cible par défaut

.PHONY: all
all: 41pass

build: 41pass

41pass: $(OBJS)
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

# Règle pour test/libc/*.c -> build/test/libc/*.o (libc, sans -I include
$(BUILD_TEST_DIR)/%.o: test/libc/%.c | $(BUILD_TEST_DIR)
	$(CC) $(MAIN_COMP_ARGS) -I include/libc -c $< -o $@

$(TEST_LIBC_BINS) : $(BUILD_TEST_DIR)/% : $(BUILD_TEST_DIR)/%.o $(LIBC_OBJS) | $(BUILD_TEST_DIR)
	$(CC) $(MAIN_COMP_ARGS) $^ -o $@

# Création des dossiers si besoin
$(BUILD_DIR) $(BUILD_SRC_DIR) $(BUILD_LIBC_DIR) $(BUILD_TEST_DIR) :
	mkdir -p $@



# Nettoyage
.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
	rm -f $(BUILD_SRC_DIR)/*.o $(BUILD_SRC_DIR)/*.d
	rm -f $(BUILD_LIBC_DIR)/*.o $(BUILD_LIBC_DIR)/*.d
	rm -f $(BUILD_TEST_DIR)/*.o $(BUILD_TEST_DIR)/*.d
	rm -rf build

.PHONY: fclean
fclean: clean
	rm -f 41pass

.PHONY: re
re: fclean all


.PHONY: test_libc
test_libc: $(TEST_LIBC_BINS) | $(BUILD_TEST_DIR)
	@echo "Running tests..."
	@failed=0; \
	for test_bin in $(TEST_LIBC_BINS); do \
		echo -n "[$(GREEN) RUN $(RESET)] $$test_bin... "; \
		if $$test_bin; then \
			echo "$(GREEN)PASS$(RESET)"; \
		else \
			echo "$(RED)FAIL$(RESET)"; \
			failed=1; \
		fi; \
	done; \
	exit $$failed

# Inclusion des fichiers de dépendances générés par -MMD -MP
-include $(OBJS:.o=.d)

-include $(TEST_LIBC_OBJS:.o=.d)

print:
	echo $(BUILD_TEST_DIR)
 