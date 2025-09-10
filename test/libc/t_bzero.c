#include "x41_errno.h"
#include "x41_unistd.h"
#include "colors.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

// Prototype for bzero (if not included from strings.h)
void x41_bzero(void *s, size_t n);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic bzero test */
static void test_bzero_basic() {
    char buf[10] = "abcdefghi";
    x41_bzero(buf, 10);
    for (int i = 0; i < 10; i++) {
        assert(buf[i] == 0);
    }
}

/* 2. Partial bzero test */
static void test_bzero_partial() {
    char buf[10] = "abcdefghi";
    x41_bzero(buf, 5);
    for (int i = 0; i < 5; i++) {
        assert(buf[i] == 0);
    }
    for (int i = 5; i < 9; i++) {
        assert(buf[i] == 'f' + (i - 5));
    }
}

/* 3. bzero with n = 0 - should do nothing */
static void test_bzero_zero_length() {
    char buf[5] = "abcd";
    x41_bzero(buf, 0);
    assert(strcmp(buf, "abcd") == 0);
}

/* 4. bzero with NULL pointer and n = 0 - should do nothing */
static void test_bzero_null_pointer() {
    // x41_bzero(NULL, 0) is allowed by POSIX, should not crash
    x41_bzero(NULL, 0);
}

/* 5. Large buffer bzero */
static void test_bzero_large() {
    char buf[1000];
    memset(buf, 'A', sizeof(buf));
    x41_bzero(buf, sizeof(buf));
    for (int i = 0; i < 1000; i++) {
        assert(buf[i] == 0);
    }
}

static void run_test(void (*test_func)(void), const char *test_name) {
    printf("Running %s:\n", test_name);
    ++tests_run;
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        test_func();
        printf(COLOR_GREEN "%s OK\n\n" COLOR_RESET, test_name);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            ++tests_passed;
        } else {
            printf(COLOR_BRED "%s KO.\n\n" COLOR_RESET, test_name);
            if (WIFSIGNALED(status)) {
                printf("Terminated by signal %d\n", WTERMSIG(status));
            } else if (WIFEXITED(status)) {
                printf("Exited with status %d\n", WEXITSTATUS(status));
            } else {
                printf("%s FAILED: Unknown termination reason\n", test_name);
            }
            ++tests_failed;
        }
    }
}

int main() {
    run_test(test_bzero_basic, "test_bzero_basic");
    run_test(test_bzero_partial, "test_bzero_partial");
    run_test(test_bzero_zero_length, "test_bzero_zero_length");
    run_test(test_bzero_null_pointer, "test_bzero_null_pointer");
    run_test(test_bzero_large, "test_bzero_large");

    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET, tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}