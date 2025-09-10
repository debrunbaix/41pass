#include "x41_errno.h"
#include "colors.h"
#include "x41_unistd.h"
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

char *x41_readline(const char *prompt);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic readline test */
static void test_basic_readline(void) {
    char *line = x41_readline("Enter text: ");
    assert(line != NULL);
    assert(x41_errno == 0);
    printf("You entered: %s\n", line);
    x41_free(line);
}

/* 2. Test readline with empty prompt */
static void test_empty_prompt(void) {
    char *line = x41_readline("");
    assert(line != NULL);
    assert(x41_errno == 0);
    printf("You entered: %s\n", line);
    x41_free(line);
}

/* 3. Test readline with NULL prompt */
static void test_null_prompt(void) {
    char *line = x41_readline(NULL);
    assert(line != NULL);
    assert(x41_errno == 0);
    printf("You entered: %s\n", line);
    x41_free(line);
}

/* 4. Test readline with long input */
static void test_long_input(void) {
    printf("Please enter a long line (over 10000 characters):\n");
    char *line = x41_readline(NULL);
    assert(line != NULL);
    assert(x41_errno == 0);
    x41_free(line);
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
            printf(COLOR_GREEN "%s OK\n\n" COLOR_RESET, test_name);
        } else {
            ++tests_failed;
            printf(COLOR_BRED "%s KO.\n\n" COLOR_RESET, test_name);
            if (WIFSIGNALED(status)) {
                printf("Terminated by signal %d\n", WTERMSIG(status));
            } else if (WIFEXITED(status)) {
                printf("Exited with status %d\n", WEXITSTATUS(status));
            } else {
                printf("%s FAILED: Unknown termination reason\n", test_name);
            }
        }
    }
}

int main(void) {

    printf(COLOR_CYAN "\n===== Starting readline() tests... =====\n" COLOR_RESET);
    run_test(test_basic_readline, "Basic readline test");
    run_test(test_empty_prompt, "Empty prompt test");
    run_test(test_null_prompt, "Null prompt test");
    run_test(test_long_input, "Long input test");
    
    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}