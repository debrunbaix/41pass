# Compilateurs / lieur
CC  := gcc
LD  := ld

# Flags
MAIN_COMP_ARGS := -fno-stack-protector -MMD -MP -g -O0 -Wall -Wextra #-fsanitize=address,leak,undefined
INCL_COMP_ARGS := $(MAIN_COMP_ARGS) -I include
LINK_ARGS      := -e _start

# Dossiers
BUILD_DIR       := build
BUILD_SRC_DIR   := $(BUILD_DIR)/src
BUILD_LIBC_DIR  := $(BUILD_SRC_DIR)/libc
BUILD_PWMAN_DIR := $(BUILD_SRC_DIR)/pwman
BUILD_TEST_DIR  := $(BUILD_DIR)/test/libc
BUILD_TEST_PWMAN_DIR  := $(BUILD_DIR)/test/pwman

# Sources & objets
MAIN_SRCS  := $(wildcard src/*.c)
LIBC_SRCS  := $(wildcard src/libc/*.c)
PWMAN_SRCS := $(wildcard src/pwman/*.c)

MAIN_OBJS  := $(patsubst src/%.c,$(BUILD_SRC_DIR)/%.o,$(MAIN_SRCS))
LIBC_OBJS  := $(patsubst src/libc/%.c,$(BUILD_LIBC_DIR)/%.o,$(LIBC_SRCS))
PWMAN_OBJS := $(patsubst src/pwman/%.c,$(BUILD_PWMAN_DIR)/%.o,$(PWMAN_SRCS))

CRT0_OBJ   := $(BUILD_DIR)/crt0.o
OBJS       := $(CRT0_OBJ) $(MAIN_OBJS) $(LIBC_OBJS) $(PWMAN_OBJS)

# Tests
TEST_LIBC_SRCS := $(patsubst %, test/libc/t_%.c, \
					printf getline open read write strcmp puts strcpy strncmp putchar putnbr close)
TEST_LIBC_OBJS  := $(patsubst %.c, $(BUILD_DIR)/%.o, $(TEST_LIBC_SRCS))
TEST_LIBC_BINS  := $(patsubst %.o, %, $(TEST_LIBC_OBJS))

# Colors
RED   := \033[0;31m
GREEN := \033[0;32m
RESET := \033[0m

# Cible par défaut
.PHONY: all
all: pwman

build: pwman

pwman: $(OBJS)
	$(LD) $(LINK_ARGS) -o $@ $(CRT0_OBJ) $(MAIN_OBJS) $(LIBC_OBJS) $(PWMAN_OBJS)

test_bin: $(MAIN_OBJS) $(LIBC_OBJS) $(PWMAN_OBJS)
	$(CC) $(MAIN_COMP_ARGS) -o $@ $^


# ---- Assemblage ----
$(CRT0_OBJ): crt0.asm | $(BUILD_DIR)
	nasm -f elf64 $< -o $@

# ---- Compilation C ----
# Règle pour src/*.c -> build/src/*.o (main, AVEC -I include maintenant)
$(BUILD_SRC_DIR)/%.o: src/%.c | $(BUILD_SRC_DIR)
	$(CC) $(INCL_COMP_ARGS) -c $< -o $@

# Règle pour src/libc/*.c -> build/src/libc/*.o (libc, avec -I include)
$(BUILD_LIBC_DIR)/%.o: src/libc/%.c | $(BUILD_LIBC_DIR)
	$(CC) $(INCL_COMP_ARGS) -c $< -o $@

# Règle pour src/pwman/*.c -> build/src/pwman/*.o (pwman, avec -I include)
$(BUILD_PWMAN_DIR)/%.o: src/pwman/%.c | $(BUILD_PWMAN_DIR)
	$(CC) $(INCL_COMP_ARGS) -c $< -o $@

# Règle pour test/libc/*.c -> build/test/libc/*.o (tests)
$(BUILD_TEST_DIR)/%.o: test/libc/%.c | $(BUILD_TEST_DIR)
	$(CC) $(MAIN_COMP_ARGS) -I include/libc -c $< -o $@

$(TEST_LIBC_BINS) : $(BUILD_TEST_DIR)/% : $(BUILD_TEST_DIR)/%.o $(LIBC_OBJS) | $(BUILD_TEST_DIR)
	$(CC) $(MAIN_COMP_ARGS) $^ -o $@ -g -fsanitize=address,leak,undefined



# Création des dossiers si besoin
$(BUILD_DIR) $(BUILD_SRC_DIR) $(BUILD_LIBC_DIR) $(BUILD_PWMAN_DIR) $(BUILD_TEST_DIR) :
	mkdir -p $@

# Tests du password manager
.PHONY: test_pwman
test_pwman: pwman
	@echo "$(GREEN)Testing password manager...$(RESET)"
	@echo "Creating test database..."
	@echo "testmaster" | ./pwman init test.db
	@echo "Adding entry..."
	@printf "testmaster\ntestpass123\n" | ./pwman add test.db github.com
	@echo "Listing entries..."
	@echo "testmaster" | ./pwman list test.db
	@echo "Getting password..."
	@echo "testmaster" | ./pwman get test.db github.com
	@rm -f test.db
	@echo "$(GREEN)Tests completed!$(RESET)"

# Nettoyage
.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
	rm -f $(BUILD_SRC_DIR)/*.o $(BUILD_SRC_DIR)/*.d
	rm -f $(BUILD_LIBC_DIR)/*.o $(BUILD_LIBC_DIR)/*.d
	rm -f $(BUILD_PWMAN_DIR)/*.o $(BUILD_PWMAN_DIR)/*.d
	rm -f $(BUILD_TEST_DIR)/*.o $(BUILD_TEST_DIR)/*.d
	rm -rf build

.PHONY: fclean
fclean: clean
	rm -f pwman

.PHONY: re
re: fclean all

.PHONY: test_libc
test_libc: $(TEST_LIBC_BINS) | $(BUILD_TEST_DIR)
	@echo "Running libc tests..."
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
	if [ $$failed -eq 0 ]; then \
		echo "$(GREEN)All tests passed!$(RESET)"; \
	else \
		echo "$(RED)Some tests failed.$(RESET)"; \
	fi

# Inclusion des fichiers de dépendances générés par -MMD -MP
-include $(OBJS:.o=.d)
-include $(TEST_LIBC_OBJS:.o=.d)

# Debug
print:
	@echo "PWMAN_SRCS: $(PWMAN_SRCS)"
	@echo "PWMAN_OBJS: $(PWMAN_OBJS)"
