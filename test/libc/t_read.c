#include"x41_errno.h"
#include"colors.h"
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#if defined(__GNUC__)
    extern __thread int x41_errno;
#else
    #error "Thread‑local storage x41_errno not implemented"
#endif

ssize_t x41_read(int fd, const void *buf, size_t count);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Read a string from STDIN (fd = 0) */
static void test_read_valid(void) {
    char buffer[100] = {0};
    ssize_t ret = x41_read(0, buffer, sizeof(buffer) - 1);
    int errsv = x41_errno;
    buffer[ret] = '\0'; // Null-terminate the string
    assert(ret >= 0 /* x41_read succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret <= (ssize_t)(sizeof(buffer) - 1) /* not overflowed*/);
    assert(strlen(buffer) == (size_t)ret /* length matches*/);
    
    printf("Read %zd bytes from stdin: '%s'\n", ret, buffer);
}

/* 2. Read a known string from a temporary file */
static void test_read_from_file(void) {
    const char msg[] = "Hello, read!";
    const char filename[] = "/tmp/testfile.txt";

    // Create and write to the temporary file
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    int errsv = errno;
    assert(fd >= 0); // open succeeded
    assert(errsv == 0); // errno is 0
    
    ssize_t ret = write(fd, msg, sizeof(msg) -1);
    errsv = x41_errno;
    assert(ret >= 0); // write succeeded
    assert(ret == (ssize_t)(sizeof(msg) -1)); // all bytes written
    assert(errsv == 0); // x41_errno is 0 
    
    ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded

    // Now read from the file
    fd = open(filename, O_RDONLY);
    errsv = errno;
    assert(fd >= 0); // open succeeded
    assert(errsv == 0); // errno is 0

    char buffer[100];
    ret = x41_read(fd, buffer, sizeof(buffer) - 1);
    errsv = x41_errno;
    buffer[ret] = '\0'; // Null-terminate the string
    assert(ret >= 0); // read succeeded
    assert(ret == (ssize_t)(sizeof(msg) -1)); // all bytes read
    assert(errsv == 0); // x41_errno is 0
    assert(strncmp(buffer, msg, (size_t)ret) == 0); // content matches

    ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded
    
    unlink(filename); // Clean up the temporary file
}

/* 3. Read from an invalid file descriptor - expected EBADF */
static void test_invalid_fd(void) {
    char buffer[10];
    ssize_t ret = x41_read(-1, buffer, sizeof(buffer) -1); // Invalid fd
    int errsv = x41_errno;
    assert(ret < 0 /* x41_read failed */);
    assert(ret == -1); // return -1 on error
    assert(errsv == EBADF); // x41_errno is EBADF
}

/* 4. Bad pointer - expected EFAULT */
static void test_bad_pointer(void) {
    ssize_t ret = x41_read(0, (void *)0x1, 10); // Invalid pointer
    int errsv = x41_errno;
    assert(ret < 0 /* x41_read failed */);
    assert(ret == -1); // return -1 on error
    assert(errsv == EFAULT); // x41_errno is EFAULT
}

/* 5. Zero-length read - should succeed and return 0 */
static void test_zero_length(void) {
    char buffer[10];
    ssize_t ret = x41_read(0, buffer, 0); // Zero length
    int errsv = x41_errno;
    assert(ret == 0 /* x41_read returns 0*/);
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 6. Read from a write-only file - expected EBADF */
static void test_file_write_only(void) {
    const char msg[] = "This won't be read";
    const char filename[] = "/tmp/test_write_only.txt";
    // Create and open the file O_WRONLY|O_CREAT|O_TRUNC
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    int errsv = errno;
    assert(fd >= 0); // open succeeded
    assert(errsv == 0); // x41_errno is 0
    
    ssize_t ret = x41_read(fd, msg, sizeof(msg) -1);
    errsv = x41_errno;
    printf("ret=%zd, x41_errno=%d, strerr=%s\n", ret, errsv, strerror(errsv));
    assert(ret < 0 /* x41_read failed */); 
    assert(ret == -1); // return -1 on error
    assert(errsv == EBADF); // x41_errno is EBADF
    
    ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded
    
    unlink(filename); // Clean up the temporary file
}

/* 7. Read with a negative count - expected EFAULT */
static void test_negative_count(void) {
    char buffer[10];
    ssize_t ret = x41_read(0, buffer, (size_t)-1); // Negative count
    int errsv = x41_errno;
    assert(ret < 0 /* x41_read failed */); 
    assert(ret == -1); // return -1 on error
    printf("ret=%zd, x41_errno=%d, strerr=%s\n", ret, errsv, strerror(errsv));
    assert(errsv == EFAULT); // x41_errno is EFAULT
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

    printf(COLOR_CYAN "\n===== Starting read() tests... =====\n" COLOR_RESET);
    run_test(test_read_valid, "test_read_valid");
    run_test(test_read_from_file, "test_read_from_file");
    run_test(test_invalid_fd, "test_invalid_fd");
    run_test(test_bad_pointer, "test_bad_pointer");
    run_test(test_zero_length, "test_zero_length");
    run_test(test_file_write_only, "test_file_write_only");
    run_test(test_negative_count, "test_negative_count");
    
    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET,
           tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}
