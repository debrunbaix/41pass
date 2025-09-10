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

size_t x41_strcspn(const char *s, const char *reject);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic test: no characters from reject in s */
static void test_basic(void) {
    const char s[] = "abcdefg";
    const char reject[] = "xyz";
    size_t ret = x41_strcspn(s, reject);
    int errsv = x41_errno;
    assert(ret == strlen(s) /* entire string length */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 2. Basic test: some characters from reject in s */
static void test_some_reject(void) {
    const char s[] = "abcdefg";
    const char reject[] = "de";
    size_t ret = x41_strcspn(s, reject);
    int errsv = x41_errno;
    assert(ret == 3 /* index of 'd' */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 3. Basic test: first character is in reject */
static void test_first_char_reject(void) {
    const char s[] = "abcdefg";
    const char reject[] = "a";
    size_t ret = x41_strcspn(s, reject);
    int errsv = x41_errno;
    assert(ret == 0 /* first char is 'a' */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 4. Basic test: empty reject string */
static void test_empty_reject(void) {
    const char s[] = "abcdefg";
    const char reject[] = "";
    size_t ret = x41_strcspn(s, reject);
    int errsv = x41_errno;
    assert(ret == strlen(s) /* entire string length */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 5. Basic test: empty s string */
static void test_empty_s(void) {
    const char s[] = "";
    const char reject[] = "abc";
    size_t ret = x41_strcspn(s, reject);
    int errsv = x41_errno;
    assert(ret == 0 /* empty string */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 6. Basic test: both strings empty */
static void test_both_empty(void) {
    const char s[] = "";
    const char reject[] = "";
    size_t ret = x41_strcspn(s, reject);
    int errsv = x41_errno;
    assert(ret == 0 /* empty string */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 7. Test with special characters */
static void test_special_chars(void) {
    const char s[] = "abc$%^&*()def";
    const char reject[] = "$&";
    size_t ret = x41_strcspn(s, reject);
    int errsv = x41_errno;
    assert(ret == 3 /* index of '$' */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 8. Test with all characters in reject */
static void test_all_reject(void) {
    const char s[] = "abc";
    const char reject[] = "abc";
    size_t ret = x41_strcspn(s, reject);
    int errsv = x41_errno;
    assert(ret == 0 /* first char is 'a' */);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 9. Test with all characters in random order in reject */
static void test_all_reject_random_order(void) {
    const char s[] = "abc";
    const char reject[] = "cab";
    size_t ret = x41_strcspn(s, reject);
    int errsv = x41_errno;
    assert(ret == 0 /* first char is 'a' */);
    assert(errsv == 0 /* x41_errno is 0 */);
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

    printf(COLOR_CYAN "\n===== Starting strcspn() tests... =====\n" COLOR_RESET);
    run_test(test_basic, "test_basic");
    run_test(test_some_reject, "test_some_reject");
    run_test(test_first_char_reject, "test_first_char_reject");
    run_test(test_empty_reject, "test_empty_reject");
    run_test(test_empty_s, "test_empty_s");
    run_test(test_both_empty, "test_both_empty");
    run_test(test_special_chars, "test_special_chars");
    run_test(test_all_reject, "test_all_reject");
    run_test(test_all_reject_random_order, "test_all_reject_random_order");

    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}
