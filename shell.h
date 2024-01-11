#define MAX_COMMANDS 100
#define MAX_ALIASES 10
#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_INPUT_SIZE 1024
#define PATH_DELIMITER ":"
#define MAX_COMMANDS 100
#define MAX_ALIASES 10

/**
 * execute_command - Execute a command in a child process
 * @command: The command to execute
 */
void execute_command(char *command);

/**
 * parse_input - Parse user input into a command and arguments
 * @input: The user input
 * @command: The resulting command
 * @args: Array to store arguments
 */
void parse_input(char *input, char *command, char *args[]);

/**
 * shell_prompt - Display the shell prompt
 */
void shell_prompt(void);

/**
 * find_command_path - Find the full path of the command using PATH
 * @command: The command to find
 * Return: The full path of the command or NULL if not found
 */
char *find_command_path(char *command);

/**
 * shell_exit - Exit the shell
 */
void shell_exit(void);

/**
 * shell_env - Print the current environment variables
 */
void shell_env(void);

/**
 * _getline - Read a line from the standard input
 * @lineptr: Pointer to the buffer containing the read line
 * @n: Number of bytes to read
 * Return: Number of bytes read
 */
ssize_t _getline(char **lineptr, size_t *n);

/**
 * tokenize_input - Tokenize the input into an array of strings
 * @input: The input string
 * Return: Array of tokens
 */
char **tokenize_input(char *input);

/**
 * strip_comments - Remove comments from a command
 * @command: The command to remove comments from
 * Return: The command without comments
 */

char *replace_variables(char *command);

/**
 * Remove comments from a command
 * @param command: The command to remove comments from
 * @return: The command without comments
 */
char *strip_comments(char *command);

/**
 * execute_logical_commands - Execute logical operators (&& and ||) in the input
 * @input: The input string
 */
void execute_logical_commands(char *input);

int execute_command_with_logical(char *command);

/**
 * handle_builtin_commands - Handle built-in commands such as exit and env
 * @command: The command to handle
 * @args: Array of arguments for the command
 */
void handle_builtin_commands(char *command, char *args[]);

/**
 * execute_file_commands - Execute commands from a file
 * @filename: The name of the file containing commands
 */
void execute_file_commands(const char *filename);

/* Structure to store aliases */
struct Alias
{
	char *name;
	char *value;
};

/* Global array to store aliases */
extern struct Alias aliases[MAX_ALIASES];

/**
 * Print aliases
 */
void print_aliases(void);
/**
 * Set or modify an alias
 * @param name: The name of the alias
 * @param value: The value of the alias
 * @return: 0 on success, -1 on failure
 */
int set_alias(char *name, char *value);
/**
 * Handle the alias command
 * @param input: The user input
 */
void shell_alias(char *input);

void run_shell(void);  /* Declaration of the main shell function */

#endif /* SHELL_H */
