#include "header.hpp"

string expandPath(const char *path, const char *base_path) {
    if (strstr(path, base_path) == path) {
        return string("~") + (path + strlen(base_path));
    }
    return path;
}




void cd_command(char *path, const char *base_path) {
    if (path == nullptr) {
        path = getenv("HOME");
    } else if (strcmp(path, "-") == 0) {
        path = getenv("OLDPWD");
    } else if (strcmp(path, "~") == 0) {
        path = getenv("HOME");
    }

    if (chdir(path) != 0) {
        perror("cd");
    }
}




void echo_command(char **args, const char *output_file) {
    // Prepare the output content
    string output_content;
    for (int i = 1; args[i] != nullptr; ++i) {
        // Check if the argument is enclosed in double quotes
        if (args[i][0] == '"' && args[i][strlen(args[i]) - 1] == '"') {
            // If enclosed in double quotes, remove the quotes and append
            output_content += args[i] + 1;
            output_content.resize(output_content.size() - 1);

            // If output redirection is specified, write to the file
            if (output_file != nullptr) {
                FILE *output_stream = fopen(output_file, "w");
                if (output_stream == nullptr) {
                    perror("Output redirection");
                    return;
                }
                fputs(output_content.c_str(), output_stream);
                fclose(output_stream);
                return; // Exit after writing to the file
            }
        } else {
            // Otherwise, append the argument as is
            output_content += args[i];
        }

        if (args[i + 1] != nullptr) {
            output_content += ' ';
        }
    }
    output_content += '\n';

    // If output redirection is specified, write to the file
    if (output_file != nullptr) {
        FILE *output_stream = fopen(output_file, "w");
        if (output_stream == nullptr) {
            perror("Output redirection");
            return;
        }
        fputs(output_content.c_str(), output_stream);
        fclose(output_stream);
    } else {
        // Otherwise, print to the standard output
        cout << output_content;
    }
}



void pwd_command() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        cout << cwd << endl;
    } else {
        perror("pwd");
    }
}

void listDirectory(const char *dirPath, bool showHidden, bool showLongFormat) {
    DIR *dir = opendir(dirPath);
    if (!dir) {
        perror("ls");
        return;
    }

    dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (!showHidden && entry->d_name[0] == '.') {
            continue;
        }

        if (showLongFormat) {
            struct stat st;
            string filePath = string(dirPath) + "/" + entry->d_name;
            if (stat(filePath.c_str(), &st) == 0) {
                struct passwd *pwd = getpwuid(st.st_uid);
                struct group *grp = getgrgid(st.st_gid);
                char timeStr[80];
                strftime(timeStr, sizeof(timeStr), "%b %d %H:%M", localtime(&st.st_mtime));

                cout << (S_ISDIR(st.st_mode) ? 'd' : '-')
                          << (st.st_mode & S_IRUSR ? 'r' : '-')
                          << (st.st_mode & S_IWUSR ? 'w' : '-')
                          << (st.st_mode & S_IXUSR ? 'x' : '-')
                          << (st.st_mode & S_IRGRP ? 'r' : '-')
                          << (st.st_mode & S_IWGRP ? 'w' : '-')
                          << (st.st_mode & S_IXGRP ? 'x' : '-')
                          << (st.st_mode & S_IROTH ? 'r' : '-')
                          << (st.st_mode & S_IWOTH ? 'w' : '-')
                          << (st.st_mode & S_IXOTH ? 'x' : '-')
                          << " " << st.st_nlink << " "
                          << (pwd ? pwd->pw_name : "unknown") << " "
                          << (grp ? grp->gr_name : "unknown") << " "
                          << st.st_size << " "
                          << timeStr << " "
                          << entry->d_name << endl;
            }
        } else {
            cout << entry->d_name << " ";
        }
    }

    closedir(dir);
    cout << endl;
}

void ls_command(char **args) {
    bool showHidden = false;
    bool showLongFormat = false;
    const char *directory = "."; // Default directory is the current directory

    // Parse command-line arguments for ls
    for (int i = 1; args[i] != nullptr; ++i) {
        if (args[i][0] == '-') {
            // Handle combined flags like -al or -la
            for (int j = 1; args[i][j] != '\0'; ++j) {
                if (args[i][j] == 'a') {
                    showHidden = true;
                } else if (args[i][j] == 'l') {
                    showLongFormat = true;
                } else {
                    cerr << "ls: invalid option -" << args[i][j] << endl;
                    return;
                }
            }
        } else {
            // Assume it's the directory path
            if (strcmp(args[i], "~") == 0) {
                directory = getenv("HOME");
            } else {
                directory = args[i];
            }
        }
    }

    // Call listDirectory to list files and directories
    listDirectory(directory, showHidden, showLongFormat);
}

void pinfo_command(char **args) {
    // Get the PID to display information about
    pid_t pid;
    if (args[1] == NULL) {
        pid = getpid(); // If no argument is provided, use the shell's PID
    } else {
        pid = atoi(args[1]); // Convert the argument to an integer (PID)
    }

    // Form the path to the process' status file
    char status_path[100];
    snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);

    // Read and display process status information
    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL) {
        perror("Error opening status file");
        return;
    }

    char line[200];
    char process_status[10] = {0}; // Buffer for process status
    while (fgets(line, sizeof(line), status_file)) {
        if (strncmp(line, "State:", 6) == 0) {
            sscanf(line + 7, "%s", process_status);
            break;
        }
    }
    fclose(status_file);

    // Check if the process is in the foreground (active)
    bool is_foreground = (getpgid(pid) == tcgetpgrp(STDIN_FILENO));

    // Display process information, adding '+' if in the foreground
    printf("pid -- %d\n", pid);
    printf("Process Status -- {%s%s}\n", process_status, is_foreground ? "+" : "");

    // Form the path to the process' statm file
    char statm_path[100];
    snprintf(statm_path, sizeof(statm_path), "/proc/%d/statm", pid);

    // Read and display memory usage
    FILE *statm_file = fopen(statm_path, "r");
    if (statm_file == NULL) {
        perror("Error opening statm file");
        return;
    }

    unsigned long size, resident, shared, text, lib, data, dt;
    if (fscanf(statm_file, "%lu %lu %lu %lu %lu %lu %lu", &size, &resident, &shared, &text, &lib, &data, &dt) != 7) {
        perror("Error reading statm file");
        fclose(statm_file);
        return;
    }
    fclose(statm_file);

    // Display memory usage in human-readable format (MB)
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024;
    long memory_mb = resident * page_size_kb / 1024;
    printf("Memory Usage -- %ld MB {Virtual Memory}\n", memory_mb);

    // Form the path to the process' executable link
    char exe_path[100];
    snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);

    // Read and display executable path
    char exec_path[PATH_MAX]; // Use PATH_MAX for dynamic allocation
    ssize_t len = readlink(exe_path, exec_path, sizeof(exec_path) - 1);
    if (len != -1) {
        exec_path[len] = '\0';
        printf("Executable Path -- %s\n", exec_path);
    } else {
        perror("Error reading executable path");
    }
}

bool search_command(const char *filename) {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        struct stat st;
        if (stat(entry->d_name, &st) == 0) {
            if (S_ISREG(st.st_mode) && strcmp(entry->d_name, filename) == 0) {
                closedir(dir);
                return true;
            }
        }
    }

    closedir(dir);
    return false;
}


vector<string> listFilesAndDirs(const char *path) {
    vector<string> entries;
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            entries.push_back(entry->d_name);
        }
        closedir(dir);
    }
    return entries;
}

string autocomplete(const string &input) {
    // Get the current directory
    const char *current_directory = ".";

    // List files and directories in the current directory
    vector<string> entries = listFilesAndDirs(current_directory);

    // Find matches for the input
    vector<string> matches;
    for (const string &entry : entries) {
        if (entry.compare(0, input.length(), input) == 0) {
            matches.push_back(entry);
        }
    }

    // If there's only one match, return it for autocompletion
    if (matches.size() == 1) {
        return matches[0];
    }

    // If there are multiple matches, concatenate them with spaces
    if (!matches.empty()) {
        string concatenatedMatches;
        for (const string &match : matches) {
            concatenatedMatches += match + " ";
        }
        return concatenatedMatches;
    }

    // If no or multiple matches, return the original input
    return input;
}



void execute_command(char **args, int background, const char *base_path) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        if (strcmp(args[0], "cd") == 0) {
            cd_command(args[1], base_path);
            exit(0);
        } else if (strcmp(args[0], "echo") == 0) {
            const char *output_file = nullptr; // Initialize the output file to nullptr

            // Check if output redirection is requested
            for (int i = 0; args[i] != nullptr; ++i) {
                if (strcmp(args[i], ">") == 0) {
                    output_file = args[i + 1]; // Set the output file to the next argument
                    break;
                }
            }

            echo_command(args, output_file);

            exit(0);
        } else if (strcmp(args[0], "pwd") == 0) {
            pwd_command();
            exit(0);
        }
        else if (strcmp(args[0], "search") == 0) {
            if (args[1] != nullptr) {
                bool found = search_command(args[1]);
                cout << (found ? "True" : "False") << endl;
           } else {
                cerr << "Usage: search <filename>" << endl;
           }
        }else if (strcmp(args[0], "ls") == 0) {
            // Handle ls command here
            ls_command(args);
            exit(0); // Exit the child process after ls_command
        }else {        
            // Redirect input/output as needed
            bool input_redirected = false;
            bool output_redirected = false;
            const char *input_file = nullptr;
            const char *output_file = nullptr;
            bool append_output = false;

            for (int i = 0; args[i] != nullptr; ++i) {
                if (strcmp(args[i], "<") == 0) {
                    // Input redirection
                    if (args[i + 1] != nullptr) {
                        input_redirected = true;
                        input_file = args[i + 1];
                    }
                } else if (strcmp(args[i], ">") == 0) {
                    // Output redirection (overwrite)
                    if (args[i + 1] != nullptr) {
                        output_redirected = true;
                        output_file = args[i + 1];
                    }
                } else if (strcmp(args[i], ">>") == 0) {
                    // Output redirection (append)
                    if (args[i + 1] != nullptr) {
                        output_redirected = true;
                        output_file = args[i + 1];
                        append_output = true;
                    }
                }
            }

            if (input_redirected) {
                FILE *input_stream = fopen(input_file, "r");
                if (input_stream == nullptr) {
                    perror("Input redirection");
                    exit(EXIT_FAILURE);
                }
                dup2(fileno(input_stream), STDIN_FILENO);
                fclose(input_stream);
            }

            if (output_redirected) {
                FILE *output_stream;
                if (append_output) {
                    output_stream = fopen(output_file, "a");
                } else {
                    output_stream = fopen(output_file, "w");
                }

                if (output_stream == nullptr) {
                    perror("Output redirection");
                    exit(EXIT_FAILURE);
                }
                dup2(fileno(output_stream), STDOUT_FILENO);
                fclose(output_stream);
            }

            // Execute the command
            execvp(args[0], args);
            // If execvp fails, print an error message
            cerr << "Unknown command: " << args[0] << endl;
            exit(EXIT_FAILURE);
        }

        // If none of the built-in commands, print an error message
        cerr << "Unknown command: " << args[0] << endl;
        exit(EXIT_FAILURE);
    } 
    else if (pid > 0) {
        // Parent process
        if (!background) {
            waitpid(pid, NULL, 0);
        } 
        else {
            cout << "[" << pid << "]" << endl; // Print the PID of the background process
        }
    }
    else {
        // Forking error
        perror("fork");
    }
}
