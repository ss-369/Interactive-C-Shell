# Interactive-C-Shell

---


## Description

This project is an interactive shell written in C/C++ that mimics the functionality of a basic POSIX shell. The shell allows users to execute commands, manage processes, and handle input/output redirections and pipelines. It supports both background and foreground processes, as well as several built-in shell commands.

## Features

### Shell Built-ins:
- **cd**: Change the current working directory.
- **pwd**: Print the current working directory.
- **echo**: Display messages, without handling escape sequences.
- **ls**: List directory contents with support for `-a` and `-l` flags.

### System Commands:
- Execute any system command like `vi`, `gedit`, etc., in the foreground or background using `&`.

### Additional Features:
- **pinfo**: Display process information for the shell or a given PID.
- **search**: Search for a file or folder in the current directory recursively.
- **I/O Redirection**: Handle input (`<`), output (`>`), and append (`>>`) redirection.
- **Pipelines**: Support command chaining using pipes (`|`).
- **Signal Handling**: Manage foreground/background processes with `CTRL-Z`, `CTRL-C`, and `CTRL-D`.
- **Auto-complete**: Pressing `TAB` will auto-complete files or directories in the current path.
- **History (BONUS)**: Track and display the last 10 commands entered by the user.

## Usage

### Running the Shell:
To compile and run the shell, follow these steps:

```bash
make
./a.out
```

### Example Shell Prompt:
```
<username>@<system_name>:<current_directory>$
```

### Built-in Command Usage:
- `cd <directory>`: Change directory.
- `pwd`: Print the working directory.
- `echo <message>`: Display a message.
- `ls [flags] [directory]`: List files with support for `-a`, `-l`, and directory listing.

### System Command Usage:
- `gedit &`: Run gedit in the background.
- `pinfo`: Display process information.

### I/O Redirection:
- `echo "Hello" > output.txt`: Redirect output to a file.
- `cat < input.txt`: Read input from a file.

### Pipelines:
- `cat file.txt | wc`: Pipe the output of `cat` to `wc`.

## Error Handling

- The shell includes error handling for invalid commands, file/directory paths, and invalid arguments for built-in commands.
- Appropriate error messages are displayed when commands cannot be executed.

## Limitations
- No support for escape sequences in `echo`.
- No handling of complex redirection such as `2>&1`.
- Internal commands like `cd`, `ls`, `pwd`, etc., do not run in the background.

## Directory Structure

```
<roll_number>_Assignment2/
├── README.md
├── makefile
├── shell.c
└── other_sources/
```

## How to Compile

To compile the code, use the `make` command, which will generate the `a.out` file:

```bash
make
```

## Assumptions
- The shell assumes valid input for the built-in commands.
- Environmental variables and complex shell features like quotes and escape sequences are not handled.


---

