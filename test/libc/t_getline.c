#include "x41_errno.h"
#include "x41_unistd.h"
#include "colors.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

ssize_t x41_getline(char **lineptr, size_t *n, int fd);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

int make_tempfile(const char *content) {
    const char *filename = "build/test/temporary_file.txt";
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    assert(fd >= 0);

    if (content) {
        ssize_t len = strlen(content);
        ssize_t ret = write(fd, content, len);
        assert(ret == len); // the content was written

        assert(lseek(fd, 0, SEEK_SET) == 0); // rewind for reading
    }

    // clean
    assert(unlink(filename) == 0);
    return fd;
}

/* 1. Basic getline test */
static void test_basic_line(void) {
   int fd = make_tempfile("Hello, world!\n");

    char *line = NULL;
    size_t len = 0;
    ssize_t read = x41_getline(&line, &len, fd);

    assert(read > 0);
    assert(strcmp(line, "Hello, world!\n") == 0);
    assert(len >= read);

    x41_free(line);
    close(fd);
}

/* 2. Test empty line */
static void test_empty_line(void) {
    int fd = make_tempfile("\n");
    char *line = NULL;
    size_t len = 0;
    ssize_t read = x41_getline(&line, &len, fd);

    assert(read > 0);
    assert(strcmp(line, "\n") == 0);    

    x41_free(line);
    close(fd);
}

/* 3. Test multiple lines */
static void test_multiple_lines(void) {
    int fd = make_tempfile("First line\nSecond line\nThird line");
    char *line = NULL;
    size_t len = 0;

    ssize_t read = x41_getline(&line, &len, fd);
    assert(read > 0);
    assert(strcmp(line, "First line\n") == 0);
    x41_free(line);

    line = NULL;
    len = 0;
    read = x41_getline(&line, &len, fd);
    assert(read > 0);
    assert(strcmp(line, "Second line\n") == 0);
    x41_free(line);

    line = NULL;
    len = 0;
    read = x41_getline(&line, &len, fd);
    assert(read > 0);
    assert(strcmp(line, "Third line") == 0);
    x41_free(line);
    close(fd);
}  

/* 4. Pointer to the line is NULL - Should return -1 and EINVAL */
static void test_null_pointer(void) {
    int fd = make_tempfile("Some content\n");
    size_t len = 0;
    ssize_t read = x41_getline(NULL, &len, fd);
    
    assert(read == -1);
    assert(x41_errno == EINVAL);
    
    x41_free(NULL); // Should do nothing and not crash

    close(fd);
}

/* 5. Size pointer is NULL - Should return -1 and EINVAL */
static void test_null_size_pointer(void) {
    int fd = make_tempfile("Some content\n");
    char *line = NULL;
    ssize_t read = x41_getline(&line, NULL, fd);
    
    assert(read == -1);
    assert(x41_errno == EINVAL);

    x41_free(line); // Should do nothing and not crash

    close(fd);
}

/* 6. Read from invalid file descriptor - Should return -1 and EBADF */
static void test_invalid_fd(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read = x41_getline(&line, &len, -1); // Invalid fd
    assert(read == -1);
    assert(x41_errno == EBADF);
    
    x41_free(line);
}

/* 7. Allocation failure simulation - Should return -1 and ENOMEM */
/**
static void test_allocation_failure(void) {
    int fd = make_tempfile("Some content\n");
    char *line = NULL;
    size_t len = 0;
    // Simulate allocation failure by requesting an extremely large size
    // Or another way to simulate allocation failure 
    // make a wrapper on malloc or realloc and on the x time make it fail
    
    
    ssize_t read = x41_getline(&line, &len, fd);
    assert(read == -1);
    assert(x41_errno == ENOMEM);
    close(fd);
    
    x41_free(line);
} */

/* 8. Test Empty file */
static void test_empty_file(void) {
    int fd = make_tempfile("");
    char *line = NULL;
    size_t len = 0;
    ssize_t read = x41_getline(&line, &len, fd);
    assert(read == -1); // EOF immediately
    assert(x41_errno == 0); // No error, just EOF
    x41_free(line);
    close(fd);
}

/* 9. Test Long line  - checks dynamic buffer growth */
static void test_long_line(void) {
    // Create a long line of 10,000 'A's followed by a newline
    char *long_line = (char *)malloc(10002);
    memset(long_line, 'A', 10000);
    long_line[10000] = '\n';
    long_line[10001] = '\0';

    int fd = make_tempfile(long_line);
    free(long_line);

    char *line = NULL;
    size_t len = 0;
    ssize_t read = x41_getline(&line, &len, fd);

    assert(read == 10001);
    assert(len >= read);
    assert(line[10000] == '\n');
    for (ssize_t i = 0; i < 10000; ++i) {
        assert(line[i] == 'A');
    }

    x41_free(line);
    close(fd);
}

/* 10. EOF without newline */
static void test_eof_without_newline(void) {
    int fd = make_tempfile("No newline at end");
    char *line = NULL;
    size_t len = 0;
    ssize_t read = x41_getline(&line, &len, fd);

    assert(read == sizeof("No newline at end") - 1); 
    assert(strcmp(line, "No newline at end") == 0);

    x41_free(line);
    close(fd);
}

/* 11. Test pipe input */
static void test_pipe_input(void) {
    int pipefd[2];
    assert(pipe(pipefd) == 0);

    const char *input = "Line from pipe\n\nThird line\n";
    write(pipefd[1], input, strlen(input));
    close(pipefd[1]); // Close write end to simulate EOF

    char *line = NULL;
    size_t len = 0;

    ssize_t read = x41_getline(&line, &len, pipefd[0]);
    assert(read == (ssize_t)strlen("Line from pipe\n"));
    assert(strcmp(line, "Line from pipe\n") == 0);

    read = x41_getline(&line, &len, pipefd[0]);
    assert(read == (ssize_t)strlen("\n"));
    assert(strcmp(line, "\n") == 0);
    
    read = x41_getline(&line, &len, pipefd[0]);
    assert(read == (ssize_t)strlen("Third line\n"));
    assert(strcmp(line, "Third line\n") == 0);

    read = x41_getline(&line, &len, pipefd[0]);
    assert(read == -1); // EOF
    assert(x41_errno == 0); // No error, just EOF

    x41_free(line);
    close(pipefd[0]);
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


// gcc -nostdlib -static -o mygetline mygetline.c

int main(void) {
    run_test(test_basic_line, "Basic getline test");
    run_test(test_empty_line, "Empty line test");
    run_test(test_multiple_lines, "Multiple lines test");
    run_test(test_null_pointer, "Null line pointer test");
    run_test(test_null_size_pointer, "Null size pointer test");
    run_test(test_invalid_fd, "Invalid file descriptor test");
    // run_test(test_allocation_failure, "Allocation failure simulation test");
    run_test(test_empty_file, "Empty file test");
    run_test(test_long_line, "Long line test");
    run_test(test_eof_without_newline, "EOF without newline test");
    run_test(test_pipe_input, "Pipe input test");

    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET, tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

