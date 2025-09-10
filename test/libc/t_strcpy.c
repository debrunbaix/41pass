#include "x41_errno.h"
#include "colors.h"
#include "x41_unistd.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#if defined(__GNUC__)
    extern __thread int x41_errno;
#else
    #error "Thread‑local storage x41_errno not implemented"
#endif

char *x41_strcpy(char *dst, const char *src);
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic copy test */
static void test_basic_copy(void) {
    const char src[] = "Hello, strcpy!";
    char dest[50] = {0};
    char *ret = x41_strcpy(dest, src);
    int errsv = x41_errno;
    assert(ret == dest /* return value is dest */);
    assert(strcmp(dest, src) == 0 /* strings match */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 2. Copy empty string */
static void test_empty_string(void) {
    const char src[] = "";
    char dest[10] = {0};
    char *ret = x41_strcpy(dest, src);
    int errsv = x41_errno;
    assert(ret == dest /* return value is dest */);
    assert(strcmp(dest, src) == 0 /* strings match */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 4. Copy with null pointers - should handle gracefully */
// static void test_null_pointers(void) {
//     char dest[10] = {0};
//     char *ret = x41_strcpy(NULL, "Non-null");
//     int errsv = x41_errno;
//     assert(ret == NULL /* return NULL on error */);
//     assert(errsv == EFAULT /* x41_errno is EFAULT */);
    
//     ret = x41_strcpy(dest, NULL);
//     errsv = x41_errno;
//     assert(ret == NULL /* return NULL on error */);
//     assert(errsv == EFAULT /* x41_errno is EFAULT */);
    
//     ret = x41_strcpy(NULL, NULL);
//     errsv = x41_errno;
//     assert(ret == NULL /* return NULL on error */);
//     assert(errsv == EFAULT /* x41_errno is EFAULT */);
// }

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
            ++tests_passed;
            printf(COLOR_GREEN "%s OK\n\n" COLOR_RESET, test_name);
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

    printf(COLOR_CYAN "\n===== Starting strcpy() tests... =====\n" COLOR_RESET);
    run_test(test_basic_copy, "test_basic_copy");
    run_test(test_empty_string, "test_empty_string");
    // run_test(test_null_pointers, "test_null_pointers");
    
    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}