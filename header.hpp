#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <vector>

using namespace std;



#define MAX_COMMANDS 50
#define MAX_COMMAND_LENGTH 100

string expandPath(const char *path, const char *base_path);
void cd_command(char *path, const char *base_path);
void echo_command(char **args, const char *output_file);
void pwd_command();
void pinfo_command(char **args);
bool search_command(const char *filename);
void execute_command(char **args, int background, const char *base_path);
void ls_command(char **args);

