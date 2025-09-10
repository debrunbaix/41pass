#include "x41_errno.h"
#include "x41_unistd.h"
#include"colors.h"
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic char test*/
static void test_basic_char(void) {
    char ch = 'H';
    int ret = x41_putchar(ch);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putchar succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 1 /* length matches */);
}

/* 2. Test newline char */
static void test_newline_char(void) {
    char ch = '\n';
    int ret = x41_putchar(ch);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putchar succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 1 /* length matches */);
}

/* 3. Test null char */
static void test_null_char(void) {
    char ch = '\0';
    int ret = x41_putchar(ch);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putchar succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 1 /* length matches */);
}

/* 4. Test non-printable char */
static void test_non_printable_char(void) {
    char ch = 0x1B; // Escape character
    int ret = x41_putchar(ch);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putchar succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 1 /* length matches */);
}

/* 5. Test high ASCII char */
static void test_high_ascii_char(void) {
    char ch = (char)0xFF; // High ASCII character
    int ret = x41_putchar(ch);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putchar succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 1 /* length matches */);
}

/* Tests runner */
void run_test(void (*test_func)(void), const char *test_name) {
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
            printf(COLOR_GREEN "%s OK\n\n" COLOR_RESET, test_name);
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

int main(void) {

    printf(COLOR_CYAN "\n===== Starting putchar() tests... =====\n" COLOR_RESET);
    run_test(test_basic_char, "test_basic_char");
    run_test(test_newline_char, "test_newline_char");
    run_test(test_null_char, "test_null_char");
    run_test(test_non_printable_char, "test_non_printable_char");
    run_test(test_high_ascii_char, "test_high_ascii_char");
    
    printf("Tests run: %d, Passed: %d, Failed: %d\n", tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    
}