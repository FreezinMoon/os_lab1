#include <fcntl.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#define BLOCK_SIZE 8192           // 8 KB
#define STACK_SIZE (1024 * 1024)  // Размер стека для clone()

struct Args {
    const char *filename;
    long repetitions;
};

int childFunction(void *arg) {
    Args *args = static_cast<Args *>(arg);
    int fd = open(args->filename, O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    void *buffer;
    posix_memalign(&buffer, BLOCK_SIZE, BLOCK_SIZE);

    for (long i = 0; i < args->repetitions; ++i) {
        lseek(fd, 0, SEEK_SET);
        ssize_t bytes_read;
        while ((bytes_read = read(fd, buffer, BLOCK_SIZE)) > 0) {
            // Ничего не делаем, просто читаем
        }
        if (bytes_read < 0) {
            perror("read failed");
            break;
        }
    }

    free(buffer);
    close(fd);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <file> <repetitions>\n";
        return EXIT_FAILURE;
    }

    Args args;
    args.filename = argv[1];
    args.repetitions = std::stol(argv[2]);

    char *stack = new char[STACK_SIZE];
    char *stackTop = stack + STACK_SIZE;

    pid_t pid = clone(childFunction, stackTop, SIGCHLD, &args);
    if (pid == -1) {
        perror("clone failed");
        delete[] stack;
        exit(EXIT_FAILURE);
    }

    waitpid(pid, nullptr, 0);

    delete[] stack;
    return 0;
}
