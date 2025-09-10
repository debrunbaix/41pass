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

int x41_strncmp(const char *s1, const char *s2, size_t n);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic comparison - equal strings up to n */
static void test_equal_strings(void) {
    const char str1[] = "Hello, World!";
    const char str2[] = "Hello, World!";
    int ret = x41_strncmp(str1, str2, sizeof(str1));
    int errsv = x41_errno;
    assert(ret == 0 /* strings are equal */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 2. Basic comparison - str1 < str2 strings up to n */
static void test_different_strings(void) {
    const char str1[] = "Hello, World A";
    const char str2[] = "Hello, World B";
    int ret = x41_strncmp(str1, str2, sizeof(str1));
    int errsv = x41_errno;
    assert(ret != 0 /* strings are different */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret < 0 /* str1 < str2 */);
}

/* 3. Basic comparison - str1 > str2 strings up to n */
static void test_different_strings_reverse(void) {
    const char str1[] = "Hello, World B";
    const char str2[] = "Hello, World A";
    int ret = x41_strncmp(str1, str2, sizeof(str1));
    int errsv = x41_errno;
    assert(ret != 0 /* strings are different */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret > 0 /* str1 > str2 */);
}

/* 4. Comparison with empty string */
static void test_empty_string(void) {
    const char str1[] = "";
    const char str2[] = "Non-empty";
    int ret = x41_strncmp(str1, str2, sizeof(str1));
    int errsv = x41_errno;
    assert(ret != 0 /* strings are different */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret < 0 /* empty string is less */);
}

/* 5. Comparison with n = 0 - should return 0 */
static void test_n_zero(void) {
    const char str1[] = "Any string";
    const char str2[] = "Another string";
    int ret = x41_strncmp(str1, str2, 0);
    int errsv = x41_errno;
    assert(ret == 0 /* n=0 means equal */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 6. Comparison with n larger than both strings - should compare full strings */
static void test_n_large(void) {
    const char str1[] = "Short";
    const char str2[] = "Shorter";
    int ret = x41_strncmp(str1, str2, 100);
    int errsv = x41_errno;
    assert(ret != 0 /* strings are different */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret < 0 /* str1 < str2 */);
}

/* 7. Comparison with n smaller than strings - should compare up to n */
static void test_n_small(void) {
    const char str1[] = "Hello, World!";
    const char str2[] = "Hello, Everyone!";
    int ret = x41_strncmp(str1, str2, 7); // Compare only "Hello, "
    int errsv = x41_errno;
    int true_ret = strncmp(str1, str2, 7);
    printf("ret=%d, true_ret=%d\n", ret, true_ret);
    assert(ret == 0 /* first 7 chars are equal */);
    assert(errsv == 0 /* x41_errno is 0 */);
    
    ret = x41_strncmp(str1, str2, 8); // Compare "Hello, W" vs "Hello, E"
    errsv = x41_errno;
    true_ret = strncmp(str1, str2, 8);
    printf("ret=%d, true_ret=%d\n", ret, true_ret);
    assert(ret != 0 /* strings differ at 8th char */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret > 0 /* 'W' > 'E' */);
}

/* 8. n = -1 */
static void test_n_negative(void) {
    const char str1[] = "Hello";
    const char str2[] = "Hello, World!";
    // size_t is unsigned, so passing -1 will be a very large number
    int ret = x41_strncmp(str1, str2, (size_t)-1);
    int errsv = x41_errno;
    
    int true_ret = strncmp(str1, str2, (size_t)-1);
    printf("ret=%d, true_ret=%d\n", ret, true_ret);
    assert(ret != 0 /* strings are different */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret < 0 /* str1 < str2 */);
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

    printf(COLOR_CYAN "\n===== Starting strncmp() tests... =====\n" COLOR_RESET);
    run_test(test_equal_strings, "test_equal_strings");
    run_test(test_different_strings, "test_different_strings");
    run_test(test_different_strings_reverse, "test_different_strings_reverse");
    run_test(test_empty_string, "test_empty_string");
    run_test(test_n_zero, "test_n_zero");
    run_test(test_n_large, "test_n_large");
    run_test(test_n_small, "test_n_small");
    run_test(test_n_negative, "test_n_negative");
    
    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}