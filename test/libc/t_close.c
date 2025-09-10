#include "x41_errno.h"
#include "colors.h"
#include "x41_fcntl.h"
#include  <errno.h>
#include  <assert.h>
#include  <fcntl.h>
#include  <unistd.h>
#include  <string.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include  <sys/stat.h>
#include  <sys/types.h>

int x41_close(int fd);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Close a valid file descriptor */
static void test_close_valid(void) {
    const char filename[] = "build/test/test_close_valid.txt";
    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0666);
    assert(fd >= 0);
    
    
    // Now close the file descriptor
    int ret = x41_close(fd);
    assert(ret == 0);
    assert(x41_errno == 0);
    
    // Clean up
    ret = unlink(filename);
    assert(ret == 0);
}

/* 2. Close an invalid file descriptor - expected EBADF */
static void test_close_invalid_fd(void) {
    int ret = x41_close(-1); // Invalid fd
    int errsv = x41_errno;
    assert(ret < 0 /* x41_close failed */);
    assert(ret == -1); // return -1 on error
    assert(errsv == EBADF); // x41_errno is EBADF
}

/* 3. Close an already closed file descriptor - expected EBADF */
static void test_close_already_closed_fd(void) {
    const char filename[] = "build/test/test_close_already_closed.txt"; 
    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0666);
    assert(fd >= 0);
    int ret = x41_close(fd);
    assert(ret == 0);
    assert(x41_errno == 0);
    ret = x41_close(fd);
    assert(ret < 0);
    assert(x41_errno == EBADF);
    ret = unlink(filename);
    assert(ret == 0);
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

    printf(COLOR_CYAN "\n===== Starting x41_close() tests... =====\n" COLOR_RESET);
    run_test(test_close_valid, "test_close_valid");
    run_test(test_close_invalid_fd, "test_close_invalid_fd");
    run_test(test_close_already_closed_fd, "test_close_already_closed_fd");
    
    printf(COLOR_CYAN "===== x41_close() tests done. =====\n" COLOR_RESET);
    printf("Tests run: %d, Passed: %d, Failed: %d\n", tests_run, tests_passed, tests_failed);
    
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}