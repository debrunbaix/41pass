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

#if defined(__GNUC__)
    extern __thread int x41_errno;
#else
    #error "Thread‑local storage x41_errno not implemented"
#endif

int x41_printf(const char *format, ...);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Basic string test */
static void test_basic_string(void) {
    const char fmt[] = "Hello, getline!\n";
    int ret = x41_printf(fmt);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -1) /* length matches */);
}

/* 2. Test percent literal %% */
static void test_percent_literal(void) {
    const char fmt[] = "100%% sure!\n";
    int ret = x41_printf(fmt);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -2) /* length matches */);
}

/* 3. Test string format %s */
static void test_string_format(void) {
    const char *str = "Test String";
    const char fmt[] = "String: %s\n";
    int ret = x41_printf(fmt, str);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + strlen(str)) /* length matches */);
}

/* 4. Test null string %s */
static void test_null_string(void) {
    const char fmt[] = "Null: %s\n";
    int ret = x41_printf(fmt, (const char *)NULL);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + strlen("(null)")) /* length matches */);
}

/* 5. Test integer format %d */
static void test_integer_format(void) {
    int num = 12345;
    const char fmt[] = "Number: %d\n";
   
    int ret = x41_printf(fmt, num);
    int errsv = x41_errno;
    char numbuf[12] = {0};
    
    snprintf(numbuf, sizeof(numbuf), "%d", num);    
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + strlen(numbuf)) /* length matches */);
}

/* 6. Test zero integer format %d */
static void test_zero_integer(void) {
    int num = 0;
    const char fmt[] = "Zero: %d\n";
    int ret = x41_printf(fmt, num);
    int errsv = x41_errno;
    char numbuf[12] = {0};
    snprintf(numbuf, sizeof(numbuf), "%d", num);
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + strlen(numbuf)) /* length matches */);
}

/* 7. Test negative integer format %d */
static void test_negative_integer(void) {
    int num = -6789;
    const char fmt[] = "Negative: %d\n";
    int ret = x41_printf(fmt, num);
    int errsv = x41_errno;
    char numbuf[12] = {0};
    snprintf(numbuf, sizeof(numbuf), "%d", num);
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + strlen(numbuf)) /* length matches */);
}

/* 8. Test int max integer format %d */
static void test_int_max_integer(void) {
    int num = 2147483647;
    const char fmt[] = "Int Max: %d\n";
    int ret = x41_printf(fmt, num);
    int errsv = x41_errno;
    char numbuf[12] = {0};
    snprintf(numbuf, sizeof(numbuf), "%d", num);
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + strlen(numbuf)) /* length matches */);
}

/* 9. Test int min integer format %d */
static void test_int_min_integer(void) {
    int num = -2147483648;
    const char fmt[] = "Int Min: %d\n";
    int ret = x41_printf(fmt, num);
    int errsv = x41_errno;
    char numbuf[12] = {0};
    snprintf(numbuf, sizeof(numbuf), "%d", num);
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + strlen(numbuf)) /* length matches */);
}

/* 10. Test overflow int max integer format %d */
static void test_overflow_int_max_integer(void) {
    long long num = 2147483648LL; // One more than INT_MAX
    const char fmt[] = "Overflow Int Max: %d\n";
    int ret = x41_printf(fmt, (int)num); // Cast to int to simulate overflow
    int errsv = x41_errno;
    char numbuf[12] = {0};
    snprintf(numbuf, sizeof(numbuf), "%d", (int)num);
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + strlen(numbuf)) /* length matches */);
}

/* 11. Test character format %c */
static void test_character_format(void) {
    char ch = 'A';
    const char fmt[] = "Char: %c\n";
    int ret = x41_printf(fmt, ch);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + 1) /* length matches */);
}

/* 12. Test null format string */
static void test_null_format_string(void) {
    int ret = x41_printf(NULL);
    int errsv = x41_errno;
    assert(ret < 0 /* x41_printf failed */); 
    assert(ret == -1); // return -1 on error
    assert(errsv == EINVAL); // x41_errno is EINVAL
}

/* 13. Test multiple formats */
static void test_multiple_formats(void) {
    const char *str = "Multi";
    int num = 42;
    char ch = 'Z';
    const char fmt[] = "String: %s, Number: %d, Char: %c\n";
    int ret = x41_printf(fmt, str, num, ch);
    int errsv = x41_errno;
    char numbuf[12] = {0};
    snprintf(numbuf, sizeof(numbuf), "%d", num);
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) - 7 + strlen(str) + strlen(numbuf) + 1) /* length matches */);
}

/* 13. Test empty format string */
static void test_empty_format_string(void) {
    const char fmt[] = "";
    int ret = x41_printf(fmt);
    int errsv = x41_errno;
    assert(ret == 0 /* x41_printf succeeded with 0 length */); 
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 14. Test '/0' character format string %c */
static void test_null_character_format(void) {
    char ch = '\0';
    const char fmt[] = "Null Char: %c End\n";
    int ret = x41_printf(fmt, ch);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (int)(sizeof(fmt) -3 + 1) /* length matches */);
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

    printf(COLOR_CYAN "\n===== Starting printf() tests... =====\n" COLOR_RESET);
    run_test(test_basic_string, "test_basic_string");
    run_test(test_percent_literal, "test_percent_literal");
    run_test(test_string_format, "test_string_format");
    run_test(test_null_string, "test_null_string");
    run_test(test_integer_format, "test_integer_format");
    run_test(test_zero_integer, "test_zero_integer");
    run_test(test_negative_integer, "test_negative_integer");
    run_test(test_int_max_integer, "test_int_max_integer");
    run_test(test_int_min_integer, "test_int_min_integer");
    run_test(test_overflow_int_max_integer, "test_overflow_int_max_integer");
    run_test(test_character_format, "test_character_format");
    run_test(test_null_format_string, "test_null_format_string");
    run_test(test_multiple_formats, "test_multiple_formats");
    run_test(test_empty_format_string, "test_empty_format_string");
    run_test(test_null_character_format, "test_null_character_format");

    printf("Tests run: %d, Passed: %d, Failed: %d\n", tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}