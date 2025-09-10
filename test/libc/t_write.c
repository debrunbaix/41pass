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

ssize_t x41_write(int fd, const void *buf, size_t count);
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Write a short string to STDOUT (fd = 1)*/
static void test_stdout(void) {
    const char msg[] = "Hello World!\n";
    ssize_t ret = x41_write(1, msg, sizeof(msg) -1);
    int errsv = x41_errno;
    assert(ret >= 0 /* x41_write succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (ssize_t)(sizeof(msg) -1) /* length matches */); 
}

/* 2. Write with a negative count */
static void test_negative_count(void) {
    const char msg[] = "Hello Write!\n";
    ssize_t ret = x41_write(1, msg, (size_t)-1);
    int errsv = x41_errno;
    assert(ret < 0); // write failed
    assert(ret == -1); // return -1 on error
    printf("ret=%zd, x41_errno=%d, strerr=%s\n", ret, errsv, strerror(errsv));
    assert(errsv == EFAULT); // x41_errno is EFAULT
}

/* 3. Write to an invalid file descriptor - expected EBADF */
static void test_invalid_fd(void) {
    const char msg[] = "won't be written\n";
    ssize_t ret = x41_write(-1, msg, sizeof(msg) -1);
    int errsv = x41_errno;
    assert(ret < 0 /* x41_write failed */); 
    assert(ret == -1); // return -1 on error
    assert(errsv == EBADF); // x41_errno is EBADF
}

/* 4. Write to a temporary file and read it back*/
static void test_file_io(void) {
    const char msg[] = "File I/O Test\n";
    const char filename[] = "build/test/test_write.txt";
    
    // Open the file in mode 0660 O_WRONLY|O_CREAT|O_TRUNC
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0660);
    int errsv = errno;
    printf("print %d", O_CREAT);
    printf("fd=%d, errsv=%zu, strerror=%s\n", fd, errsv, strerror(errsv));
    assert(fd >= 0); // open succeeded
    assert(errsv == 0); // x41_errno is 0
    
    ssize_t ret = x41_write(fd, msg, sizeof(msg) -1);
    errsv = x41_errno;
    assert(ret >= 0); // write succeeded
    assert(ret == (ssize_t)(sizeof(msg) -1)); // all bytes written
    assert(errsv == 0); // x41_errno is 0 
    
    ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded
    
    // Reopen the file for reading
    fd = open(filename, O_RDONLY);
    errsv = errno;
    assert(fd >= 0); // open succeeded
    assert(errsv == 0); // x41_errno is 0
    
    char buffer[64] = {0};
    ret = read(fd, buffer, sizeof(buffer) -1);
    errsv = x41_errno;
    buffer[ret] = '\0'; // Null-terminate the string
    assert(ret >= 0 /* x41_read succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    assert(ret == (ssize_t)(sizeof(msg) -1)); // all bytes read
    assert(ret <= (ssize_t)(sizeof(buffer) - 1) /* not overflowed*/);
    assert(strncmp(buffer, msg, (size_t)ret) == 0); // content matches

    ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded
    
    unlink(filename); // Clean up the temporary file
}

/* 5. Bad pointer - expected EFAULT */
static void test_bad_pointer(void) {
    ssize_t ret = x41_write(1, (const void *)0x1, 10); // Invalid pointer
    int errsv = x41_errno;
    assert(ret < 0 /* x41_write failed */); 
    assert(ret == -1); // return -1 on error
    assert(errsv == EFAULT); // x41_errno is EFAULT
}
/* 6. Zero-length write - should succed and return 0 */
static void test_zero_length(void) {
    const char msg[] = "Hello Zero Length!\n";
    ssize_t ret = x41_write(1, msg, 0); // Zero length
    int errsv = x41_errno;
    assert(ret == 0); // write returns 0
    assert(errsv == 0 /* x41_errno is 0 */);
}

/* 7. Write to a  read-only file and read it back*/
static void test_file_read_only(void) {
    const char msg[] = "Read-only File I/O Test\n";
    const char filename[] = "build/test/test_write.txt";
    
    // Open the file O_RDONLY|O_CREAT|O_TRUNC
    int fd = open(filename, O_RDONLY | O_CREAT | O_TRUNC);
    int errsv = errno;
    assert(fd >= 0); // open succeeded
    assert(errsv == 0); // x41_errno is 0
    
    ssize_t ret = x41_write(fd, msg, sizeof(msg) -1);
    errsv = x41_errno;
    printf("ret=%zd, x41_errno=%d, strerr=%s\n", ret, errsv, strerror(errsv));
    assert(ret < 0 /* x41_write failed */); 
    assert(ret == -1); // return -1 on error
    assert(errsv == EBADF); // x41_errno is EBADF
    
    ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded
    
    unlink(filename); // Clean up the temporary file
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

    printf(COLOR_CYAN "\n===== Starting write() tests... =====\n" COLOR_RESET);
    run_test(test_stdout, "test_stdout");
    run_test(test_negative_count, "test_negative_count");
    run_test(test_invalid_fd, "test_invalid_fd");
    run_test(test_file_io, "test_file_io");
    run_test(test_bad_pointer, "test_bad_pointer");
    run_test(test_zero_length, "test_zero_length");
    run_test(test_file_read_only, "test_file_read_only");
    printf("Tests run: %d, Passed: %d, Failed: %d\n", tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    
}
