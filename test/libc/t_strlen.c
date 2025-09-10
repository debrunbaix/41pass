#include "x41_errno.h"
#include "x41_unistd.h"
#include "colors.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h> 
#include <sys/stat.h>
#include <fcntl.h>

size_t x41_strlen(const char *s);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic test: normal string */
static void test_basic(void) {
    const char s[] = "Hello, world!";
    size_t ret = x41_strlen(s);
    int errsv = x41_errno;
    assert(ret == strlen(s) /* length matches */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 2. Basic test: empty string */
static void test_empty_string(void) {
    const char s[] = "";
    size_t ret = x41_strlen(s);
    int errsv = x41_errno;
    assert(ret == 0 /* length is 0 */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 3. Basic test: long string */
static void test_long_string(void) {
    const char s[] = "This is a longer string used for testing the strlen function implementation in the custom libc.";
    size_t ret = x41_strlen(s);
    int errsv = x41_errno;
    assert(ret == strlen(s) /* length matches */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 4. Basic test: string with special characters */
static void test_special_chars(void) {
    const char s[] = "Hello, \n\tWorld! \0 Hidden";
    size_t ret = x41_strlen(s);
    int errsv = x41_errno;
    assert(ret == sizeof("Hello, \n\tWorld! ") - 1 /* length up to null terminator */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 5. NULL test: should handle gracefully */
// static void test_null_pointer(void) {
//     size_t ret = x41_strlen(NULL);
//     int errsv = x41_errno;
//     assert(ret == 0 /* return 0 on NULL */);
//     assert(errsv == EFAULT /* x41_errno is EFAULT */);
// }



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

    printf(COLOR_CYAN "\n===== Starting strlen() tests... =====\n" COLOR_RESET);
    run_test(test_basic, "test_basic");
    run_test(test_empty_string, "test_empty_string");
    run_test(test_long_string, "test_long_string");
    run_test(test_special_chars, "test_special_chars");
    // run_test(test_null_pointer, "test_null_pointer");
    
    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}