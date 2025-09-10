#include "x41_errno.h"
#include "colors.h"
#include "x41_stdarg.h"
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

char *x41_strdup(const char *s);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic strdup test */
static void test_basic_strdup(void) {
    const char str[] = "Hello, strdup!";
    char *dup = x41_strdup(str);
    int errsv = x41_errno;
    assert(dup != NULL /* strdup succeeded */);
    assert(strcmp(dup, str) == 0 /* strings match */);
    assert(errsv == 0 /* x41_errno is 0 */);
    x41_free(dup);
}

/* 2. strdup empty string */
static void test_empty_string(void) {
    const char str[] = "";
    char *dup = x41_strdup(str);
    int errsv = x41_errno;
    assert(dup != NULL /* strdup succeeded */);
    assert(strcmp(dup, str) == 0 /* strings match */);
    assert(errsv == 0 /* x41_errno is 0 */);
    x41_free(dup);
}

/* 3. strdup NULL pointer - should handle gracefully */
static void test_null_pointer(void) {
    char *dup = x41_strdup(NULL);
    int errsv = x41_errno;
    assert(dup == NULL /* strdup failed */);
    assert(errsv == EFAULT /* x41_errno is EFAULT */);
}

/* 4. strdup large string */
static void test_large_string(void) {
    size_t len = 100000; // 100k characters
    char *large_str = (char *)malloc(len + 1);
    assert(large_str != NULL /* malloc succeeded */);
    memset(large_str, 'A', len);
    large_str[len] = '\0';
    char *dup = x41_strdup(large_str);
    int errsv = x41_errno;
    assert(dup != NULL /* strdup succeeded */);
    assert(strcmp(dup, large_str) == 0 /* strings match */);
    assert(errsv == 0 /* x41_errno is 0 */);
    x41_free(dup);
    free(large_str);
}

/* 5. strdup string with special characters */
static void test_special_characters(void) {
    const char str[] = "Hello,\nWorld!\tSpecial chars: \x01\x02\x03";
    char *dup = x41_strdup(str);
    int errsv = x41_errno;
    assert(dup != NULL /* strdup succeeded */);
    assert(strcmp(dup, str) == 0 /* strings match */);
    assert(errsv == 0 /* x41_errno is 0 */);
    x41_free(dup);
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

    printf(COLOR_CYAN "\n===== Starting strdup() tests... =====\n" COLOR_RESET);
    run_test(test_basic_strdup, "test_basic_strdup");
    run_test(test_empty_string, "test_empty_string");
    run_test(test_null_pointer, "test_null_pointer");
    run_test(test_large_string, "test_large_string");
    run_test(test_special_characters, "test_special_characters");
    
    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}