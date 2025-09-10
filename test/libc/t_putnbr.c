#include "x41_errno.h"
#include "x41_unistd.h"
#include "colors.h"
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

int x41_putnbr(int num);


static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic number test */
static void test_basic_number(void) {
    int num = 12345;
    int ret = x41_putnbr(num);
    int errsv = x41_errno;
    printf("ret=%d, x41_errno=%d\n", ret, errsv);
    assert(ret >= 0 /* x41_putnbr succeeded */);
    assert(ret == 5 /* length matches */);

}

/* 2. Test zero number */
static void test_zero_number(void) {
    int num = 0;
    int ret = x41_putnbr(num);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putnbr succeeded */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 1 /* length matches */);

}

/* 3. Test negative number */
static void test_negative_number(void) {
    int num = -6789;
    int ret = x41_putnbr(num);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putnbr succeeded */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 5 /* length matches */);
}

/* 4. Test int max number */
static void test_int_max_number(void) {
    int num = 2147483647;
    int ret = x41_putnbr(num);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putnbr succeeded */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 10 /* length matches */);
}

/* 5. Test int min number */
static void test_int_min_number(void) {
    int num = -2147483648;
    int ret = x41_putnbr(num);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putnbr succeeded */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 11 /* length matches */); 
}

/* 6. Double test random number */
static void test_random_number(void) {
    int num = -10;
    int ret = x41_putnbr(num);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_putnbr succeeded */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 3 /* length matches */);
        
    int num2 = 10;
    ret = x41_putnbr(num2);
    errsv = x41_errno;
    assert(ret >= 0 /* x41_putnbr succeeded */);
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == 2 /* length matches */);    
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

    printf(COLOR_CYAN "\n===== Starting putnbr() tests... =====\n" COLOR_RESET);
    run_test(test_basic_number, "test_basic_number");
    run_test(test_zero_number, "test_zero_number");
    run_test(test_negative_number, "test_negative_number");
    run_test(test_int_max_number, "test_int_max_number");
    run_test(test_int_min_number, "test_int_min_number");
    run_test(test_random_number, "test_random_number");
    
    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}   
