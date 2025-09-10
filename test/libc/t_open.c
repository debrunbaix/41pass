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

int x41_open(const char *pathname, int flags, mode_t mode);

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 1. Successful open of a newly created temporary file */
static void test_create_and_open(void) {
    const char filename[] = "build/test/test_open_valid.txt";
    int fd = x41_open(filename, O_CREAT | O_RDWR | O_TRUNC, 0600);
    int errsv = x41_errno;
    assert(fd >= 0 /* x41_open succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    
    int ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded
    
    
    /* Re-open read-only to verify the file exists */
    fd = x41_open(filename, O_RDONLY, 0);
    errsv = x41_errno;
    assert(fd >= 0 /* x41_open succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    
    ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded  
    
    // Clean up
    ret = unlink(filename);
    errsv = errno;
    assert(ret == 0); // unlink succeeded
    assert(errsv == 0); // unlink succeeded
}

/* 2. Open a non-existent file without O_CREAT - expected ENOENT */
static void test_open_nonexistent(void) {
    const char filename[] = "build/test/nonexistent_file.txt";
    int fd = x41_open(filename, O_RDWR, 0);
    int errsv = x41_errno;
    assert(fd < 0 /* x41_open failed */); 
    assert(fd == -1); // return -1 on error
    printf("fd=%d, x41_errno=%d, strerr=%s\n",  fd, errsv, strerror(errsv));
    assert(errsv == ENOENT); // x41_errno is ENOENT
}

/* 3. Test flags only - should succeed and return a fd */
static void test_open_flags_only(void) {
    // Opening /dev/null is safe and always present */
    int fd = x41_open("/dev/null",  /* O_RDONLY */ 0 , 0);
    int errsv = x41_errno;
    assert(fd >= 0 /* x41_open succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    

    // Clean up
    int ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded
}

/* 4. Invalid flags combination - e.g., O_DIRECTORY on a regular file */
static void test_open_invalid_flags_combination(void) {
    const char filename[] = "build/test/test_open_invalid_flags_combination.txt";
    // First, create a regular file
    int fd = x41_open(filename, O_CREAT | O_RDWR | O_TRUNC, 0600);
    int errsv = x41_errno;
    assert(fd >= 0 /* x41_open succeeded */); 
    assert(errsv == 0 /* x41_errno is 0 */);
    
    // Close up
    int ret = close(fd);
    errsv = errno;
    assert(ret == 0); // close succeeded
    assert(errsv == 0); // close succeeded
    
    
    // Now try to open it with O_DIRECTORY (shoulf fail with ENOTDIR)
    fd = x41_open(filename, O_DIRECTORY | O_RDONLY, 0);
    errsv = x41_errno;
    assert(fd < 0 /* x41_open failed */); 
    assert(fd == -1); // return -1 on error
    printf("fd=%d, x41_errno=%d, strerr=%s\n", fd, errsv, strerror(errsv));
    assert(errsv == ENOTDIR); // x41_errno is ENOTDIR
    

    ret = unlink(filename);
    errsv = errno;
    assert(ret == 0); // unlink succeeded
    assert(errsv == 0); // unlink succeeded
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
    run_test(test_create_and_open, "test_create_and_open");
    run_test(test_open_nonexistent, "test_open_nonexistent");
    run_test(test_open_flags_only, "test_open_flags_only");
    run_test(test_open_invalid_flags_combination, "test_open_invalid_flags_combination");

    printf(COLOR_CYAN "\n===== Summary: %d tests run, %d passed, %d failed =====\n" COLOR_RESET, tests_run, tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
