#include <fcntl.h>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

#define STACK_SIZE (1024 * 1024)

struct Command {
    std::vector<char *> args;
    std::string inputFile;
    std::string outputFile;
    bool append = false;
};

int childFunction(void *arg) {
    Command *command = static_cast<Command *>(arg);

    if (!command->inputFile.empty()) {
        int fd = open(command->inputFile.c_str(), O_RDONLY);
        if (fd == -1) {
            perror("open input file");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (!command->outputFile.empty()) {
        int flags = O_WRONLY | O_CREAT | (command->append ? O_APPEND : O_TRUNC);
        int fd = open(command->outputFile.c_str(), flags, 0644);
        if (fd == -1) {
            perror("open output file");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    if (execvp(command->args[0], command->args.data()) == -1) {
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void executePipeline(std::vector<Command> &commands) {
    std::string tempFilePrefix = "/tmp/shell_pipe_";
    std::string inputTempFile;
    std::string outputTempFile;

    for (size_t i = 0; i < commands.size(); ++i) {
        if (i > 0) {
            inputTempFile = tempFilePrefix + std::to_string(i - 1);
            commands[i].inputFile = inputTempFile;
        }
        if (i < commands.size() - 1) {
            outputTempFile = tempFilePrefix + std::to_string(i);
            commands[i].outputFile = outputTempFile;
            commands[i].append = false;
        }

        char *stack = new char[STACK_SIZE];
        char *stackTop = stack + STACK_SIZE;
        pid_t pid = clone(childFunction, stackTop, SIGCHLD, &commands[i]);
        if (pid == -1) {
            perror("clone failed");
            delete[] stack;
            exit(EXIT_FAILURE);
        }
        waitpid(pid, nullptr, 0);
        delete[] stack;

        if (!inputTempFile.empty()) {
            unlink(inputTempFile.c_str());
        }
    }

    if (!outputTempFile.empty()) {
        unlink(outputTempFile.c_str());
    }
}

bool handleBuiltInCommands(char *args[]) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == nullptr) {
            std::cerr << "cd: expected argument\n";
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd failed");
            }
        }
        return true;
    }
    if (strcmp(args[0], "help") == 0) {
        std::cout << "Available commands:\n";
        std::cout << "cd <directory> - Change the current directory\n";
        std::cout << "help - Display this help message\n";
        std::cout << "exit - Exit the shell\n";
        return true;
    }
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    return false;
}

void parseAndExecuteCommand(const std::string &line) {
    std::istringstream stream(line);
    std::string token;
    Command command;
    std::vector<Command> pipelineCommands;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (stream >> token) {
        if (token == ">") {
            stream >> command.outputFile;
            command.append = false;
        } else if (token == ">>") {
            stream >> command.outputFile;
            command.append = true;
        } else if (token == "<") {
            stream >> command.inputFile;
        } else if (token == "|") {
            command.args.push_back(nullptr);
            pipelineCommands.push_back(command);
            command = Command();
        } else {
            char *arg = new char[token.size() + 1];
            std::strcpy(arg, token.c_str());
            command.args.push_back(arg);
        }
    }

    if (!command.args.empty()) {
        command.args.push_back(nullptr);
        pipelineCommands.push_back(command);
    }

    if (pipelineCommands.size() > 1) {
        executePipeline(pipelineCommands);
    } else if (!command.args.empty()) {
        char *stack = new char[STACK_SIZE];
        char *stackTop = stack + STACK_SIZE;
        pid_t pid = clone(childFunction, stackTop, SIGCHLD, &command);
        if (pid == -1) {
            perror("clone failed");
            delete[] stack;
            exit(EXIT_FAILURE);
        }
        waitpid(pid, nullptr, 0);
        delete[] stack;
    }

    for (auto &cmd : pipelineCommands) {
        for (char *arg : cmd.args) {
            delete[] arg;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed =
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Elapsed time: %f seconds\n", elapsed);
}

int main() {
    std::string line;
    char cwd[1024];

    while (true) {
        if (getcwd(cwd, sizeof(cwd)) == nullptr) {
            perror("getcwd failed");
            strcpy(cwd, "unknown");
        }
        std::cout << cwd << "> ";
        std::getline(std::cin, line);

        if (line.empty()) continue;

        const int max_args = 64;
        char *args[max_args];
        int arg_count = 0;
        std::istringstream stream(line);
        std::string token;

        while (stream >> token) {
            args[arg_count] = new char[token.size() + 1];
            std::strcpy(args[arg_count++], token.c_str());
        }
        args[arg_count] = nullptr;

        if (handleBuiltInCommands(args)) {
            for (int i = 0; i < arg_count; ++i) {
                delete[] args[i];
            }
            continue;
        }

        parseAndExecuteCommand(line);

        for (int i = 0; i < arg_count; ++i) {
            delete[] args[i];
        }
    }
    return 0;
}