#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define BLOCK_SIZE 8192 // 8 KB

void io_read_task(const std::string &filename, long repetitions) {
  int fd = open(filename.c_str(), O_RDONLY | O_DIRECT);
  if (fd < 0) {
    perror("open failed");
    exit(EXIT_FAILURE);
  }

  void *buffer;
  if (posix_memalign(&buffer, BLOCK_SIZE, BLOCK_SIZE) != 0) {
    perror("posix_memalign failed");
    close(fd);
    exit(EXIT_FAILURE);
  }

  for (long i = 0; i < repetitions; ++i) {

    if (lseek(fd, 0, SEEK_SET) == -1) {
      perror("lseek failed");
      break;
    }

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
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0]
              << " <file> <repetitions> <num_threads>\n";
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];
  long repetitions = std::stol(argv[2]);
  int num_threads = std::stoi(argv[3]);

  struct stat statbuf;
  if (stat(filename.c_str(), &statbuf) != 0) {
    perror("stat failed");
    return EXIT_FAILURE;
  }

  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(io_read_task, filename, repetitions);
  }

  for (auto &t : threads) {
    t.join();
  }

  return 0;
}
