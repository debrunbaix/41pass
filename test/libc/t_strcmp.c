#include "x41_errno.h"
#include "colors.h"
#include "test.h"
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

int x41_strcmp(const char *s1, const char *s2);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic comparison - equal strings */
static void test_equal_strings(void) {
    const char str1[] = "Hello, World!";
    const char str2[] = "Hello, World!";
    int ret = x41_strcmp(str1, str2);
    int errsv = x41_errno;
    assert(ret == 0 /* strings are equal */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 2. Basic comparison - str1 < str2 strings */
static void test_different_strings(void) {
    const char str1[] = "Hello, World!";
    const char str2[] = "Hello, World?";
    int ret = x41_strcmp(str1, str2);
    int errsv = x41_errno;
    assert(ret != 0 /* strings are different */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret < 0 /* str1 < str2 */);
    
    int true_ret = strcmp(str1, str2);
    assert(ret == true_ret /* matches standard strcmp */);
}

/* 3. Basic comparison - str1 < str2 strings */
static void test_different_strings_reverse(void) {
    const char str2[] = "Hello, World!";
    const char str1[] = "Hello, World?";
    int ret = x41_strcmp(str1, str2);
    int errsv = x41_errno;
    assert(ret != 0 /* strings are different */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret < 0 /* str1 < str2 */);
    
    int true_ret = strcmp(str1, str2);
    assert(ret == true_ret /* matches standard strcmp */);
}

/* 4. Comparison with empty string */
static void test_empty_string(void) {
    const char str1[] = "";
    const char str2[] = "Non-empty";
    int ret = x41_strcmp(str1, str2);
    int errsv = x41_errno;
    assert(ret != 0 /* strings are different */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret < 0 /* empty string is less */);
    
    int true_ret = strcmp(str1, str2);
    assert(ret == true_ret /* matches standard strcmp */);
}

/* 5. Comparison with null pointers - should handle gracefully */
static void test_null_pointers(void) {
    int ret = x41_strcmp(NULL, "Non-null");
    int errsv = x41_errno;
    assert(ret < 0 /* NULL is less */);
    assert(errsv == EFAULT /* x41_errno is EFAULT */);
    
    ret = x41_strcmp("Non-null", NULL);
    errsv = x41_errno;
    assert(ret > 0 /* Non-null is greater */);
    assert(errsv == EFAULT /* x41_errno is EFAULT */);
    
    ret = x41_strcmp(NULL, NULL);
    errsv = x41_errno;
    assert(ret == 0 /* both NULL are equal */);
    assert(errsv == EFAULT /* x41_errno is EFAULT */);
}

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

    printf(COLOR_CYAN "\n===== Starting strcmp() tests... =====\n" COLOR_RESET);
    run_test(test_equal_strings, "test_equal_strings");
    run_test(test_different_strings, "test_different_strings");
    run_test(test_different_strings_reverse, "test_different_strings_reverse");
    run_test(test_empty_string, "test_empty_string");
    run_test(test_null_pointers, "test_null_pointers");

    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}
