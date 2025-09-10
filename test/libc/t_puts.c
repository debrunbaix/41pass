#include "x41_errno.h"
#include "colors.h"
#include "x41_unistd.h"
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#if defined(__GNUC__)
    extern __thread int x41_errno;
#else
    #error "Thread‑local storage x41_errno not implemented"
#endif

int x41_puts(const char *str);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic string test */
static void test_basic_string(void) {
    const char str[] = "Hello, puts!";
    int ret = x41_puts(str);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_puts succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(str)) /* length matches */); // +1 for newline
}

/* 2. Test empty string */
static void test_empty_string(void) {
    const char str[] = "";
    int ret = x41_puts(str);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_puts succeeded */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 1 /* length matches */); // 1 for newline
}

/* 3. Test null pointer */
static void test_null_pointer(void) {
    int ret = x41_puts(NULL);
    int errsv = x41_errno;
    assert(ret < 0 /* x41_puts failed */);
    assert(ret == -1); // return -1 on error
    assert(errsv == EFAULT); // x41_errno is EFAULT
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

int main(void) {

    printf(COLOR_CYAN "\n===== Starting puts() tests... =====\n" COLOR_RESET);
    run_test(test_basic_string, "test_basic_string");
    run_test(test_empty_string, "test_empty_string");
    run_test(test_null_pointer, "test_null_pointer");
    
    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}


