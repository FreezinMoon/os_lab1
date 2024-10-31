#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unordered_set>
#include <vector>

#define BLOCK_SIZE 8192                // 8 KB
#define DATA_SIZE (BLOCK_SIZE * 1000)  // ~8 MB
#define STACK_SIZE (1024 * 1024)       // Размер стека для clone()

struct Args {
    long repetitions;
};

struct BlockHash {
    size_t operator()(const std::vector<char> &block) const {
        return std::hash<std::string>()(
            std::string(block.begin(), block.end()));
    }
};

struct BlockEqual {
    bool operator()(const std::vector<char> &a,
                    const std::vector<char> &b) const {
        return a == b;
    }
};

int childFunction(void *arg) {
    Args *args = static_cast<Args *>(arg);

    char *data = new char[DATA_SIZE];

    // Инициализация данных некоторым паттерном
    for (size_t i = 0; i < DATA_SIZE; ++i) {
        data[i] = static_cast<char>(rand() % 256);
    }

    for (long rep = 0; rep < args->repetitions; ++rep) {
        std::unordered_set<std::vector<char>, BlockHash, BlockEqual>
            unique_blocks;

        for (size_t i = 0; i < DATA_SIZE; i += BLOCK_SIZE) {
            std::vector<char> block(data + i, data + i + BLOCK_SIZE);
            unique_blocks.insert(block);
        }

        // Обработка уникальных блоков
    }

    delete[] data;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <repetitions>\n";
        return EXIT_FAILURE;
    }

    Args args;
    args.repetitions = std::stol(argv[1]);

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
