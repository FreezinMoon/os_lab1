#define _GNU_SOURCE
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define STACK_SIZE (1024 * 1024)

int childFunction(void *arg) {
  char **args = (char **)arg;
  if (execvp(args[0], args) == -1) {
    perror("execvp failed");
    exit(EXIT_FAILURE);
  }
  return 0;
}

void executeCommand(char *args[]) {

  char *stack = (char *)malloc(STACK_SIZE);
  if (!stack) {
    perror("Failed to allocate stack");
    exit(EXIT_FAILURE);
  }
  char *stackTop = stack + STACK_SIZE;

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  pid_t pid = clone(childFunction, stackTop, SIGCHLD, args);
  if (pid == -1) {
    perror("clone failed");
    free(stack);
    exit(EXIT_FAILURE);
  }

  if (waitpid(pid, nullptr, 0) == -1) {
    perror("waitpid failed");
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsed =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  printf("Elapsed time: %f seconds\n", elapsed);

  free(stack);
}

bool handleBuiltInCommands(char *args[]) {
  if (strcmp(args[0], "cd") == 0) {
    if (args[1] == nullptr) {
      fprintf(stderr, "cd: expected argument\n");
    } else {
      if (chdir(args[1]) != 0) {
        perror("cd failed");
      }
    }
    return true;
  } else if (strcmp(args[0], "help") == 0) {
    printf("Available commands:\n");
    printf("cd <directory> - Change the current directory\n");
    printf("help - Display this help message\n");
    printf("exit - Exit the shell\n");
    return true;
  } else if (strcmp(args[0], "exit") == 0) {
    exit(0);
  }
  return false;
}

int main() {
  char line[1024];
  char cwd[1024];
  while (true) {

    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
      perror("getcwd failed");
      strcpy(cwd, "unknown");
    }
    printf("%s> ", cwd);
    if (!fgets(line, sizeof(line), stdin)) {
      break;
    }

    line[strcspn(line, "\n")] = '\0';

    if (strlen(line) == 0) {
      continue;
    }

    const int max_args = 64;
    char *args[max_args];
    int arg_count = 0;

    char *token = strtok(line, " ");
    while (token != nullptr && arg_count < max_args - 1) {
      args[arg_count++] = token;
      token = strtok(nullptr, " ");
    }
    args[arg_count] = nullptr;

    if (handleBuiltInCommands(args)) {
      continue;
    }

    executeCommand(args);
  }
  return 0;
}