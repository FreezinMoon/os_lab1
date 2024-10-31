#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include <unordered_set>
#include <vector>

#define BLOCK_SIZE 8192                // 8 KB
#define DATA_SIZE (BLOCK_SIZE * 1000)  // ~8 MB

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

void dedupTask(long repetitions) {
    char *data = new char[DATA_SIZE];

    // Инициализация данных
    for (size_t i = 0; i < DATA_SIZE; ++i) {
        data[i] = static_cast<char>(rand() % 256);
    }

    for (long rep = 0; rep < repetitions; ++rep) {
        std::unordered_set<std::vector<char>, BlockHash, BlockEqual>
            unique_blocks;

        for (size_t i = 0; i < DATA_SIZE; i += BLOCK_SIZE) {
            std::vector<char> block(data + i, data + i + BLOCK_SIZE);
            unique_blocks.insert(block);
        }
    }

    delete[] data;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <repetitions>\n";
        return EXIT_FAILURE;
    }

    long repetitions = std::stol(argv[1]);

    unsigned int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < numThreads; ++i) {
        threads.emplace_back(dedupTask, repetitions);
    }

    for (auto &t : threads) {
        t.join();
    }

    return 0;
}
