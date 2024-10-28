#include "header.hpp"

int main() {
    //const int MAX_COMMAND_LENGTH = 1024;
    char *command = new char[MAX_COMMAND_LENGTH];
    char base_path[PATH_MAX];

    if (getcwd(base_path, sizeof(base_path)) == nullptr) {
        perror("getcwd");
        delete[] command;
        return 1;
    }

    while (true) {
        char *cwd = getcwd(nullptr, 0);
        if (cwd == nullptr) {
            perror("getcwd");
            delete[] command;
            return 1;
        }

        char *user = getenv("USER");
        char hostname[1024];
        gethostname(hostname, 1024);

        string cwdStr = expandPath(cwd, base_path);

        cout << user << "@" << hostname << ":" << cwdStr << "> ";
        free(cwd);

        // Read the user input
        cin.getline(command, MAX_COMMAND_LENGTH);

        // Remove newline character from the end
        command[strlen(command)] = '\0';


        // Tokenize the input command
        char *args[MAX_COMMANDS];
        int arg_count = 0;
        args[arg_count++] = strtok(command, " ");

        while (args[arg_count - 1] != nullptr) {
            args[arg_count++] = strtok(nullptr, " ");
        }
        if (arg_count > 0) {
            if (strcmp(args[0], "exit") == 0) {
                break;
            } else if (strcmp(args[0], "cd") == 0) {
                cd_command(args[1], base_path);
            } else if (strcmp(args[0], "echo") == 0) {
                const char *output_file = nullptr;
                for (int i = 0; args[i] != nullptr; ++i) {
                    if (strcmp(args[i], ">") == 0) {
                        output_file = args[i + 1];
                        break;
                    }
                }
                echo_command(args, output_file);
            } else if (strcmp(args[0], "pwd") == 0) {
                pwd_command();
            } else if (strcmp(args[0], "pinfo") == 0) {
                pinfo_command(args);
            } else if (strcmp(args[0], "search") == 0) {
                if (args[1] != nullptr) {
                    bool found = search_command(args[1]);
                    cout << (found ? "True" : "False") << endl;
                } else {
                    cerr << "Usage: search <filename>" << endl;
                }
            } else if (strcmp(args[0], "ls") == 0) {
                // Call the ls_command function with arguments
                ls_command(args);
            } else if (args[arg_count - 2] != nullptr && strcmp(args[arg_count - 2], "&") == 0) {
                // Background process
                args[arg_count - 2] = nullptr; // Remove the "&" from the arguments
                execute_command(args, 1, base_path); // Pass base_path as an argument
            }else {
                // Execute other system commands as before
                execute_command(args, 0, base_path);
            }
        }
    }

    delete[] command;
    return 0;
}