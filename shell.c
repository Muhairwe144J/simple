#include "shell.h"

/**
 * execute_command - Execute a command in a child process
 * @command: The command to execute
 */
void execute_command(char *command)
{
	pid_t pid = fork();

	if (pid == -1)
	{
		perror("fork");
		_exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		char *command_path = find_command_path(command);

		if (command_path == NULL)
		{
			fprintf(stderr, "%s: command not found\n", command);
			_exit(EXIT_FAILURE);
		}

		if (execve(command_path, (char *const *)&command, NULL) == -1)
		{
			perror("execve");
			_exit(EXIT_FAILURE);
		}
	}
	else
	{
		wait(NULL);
	}
}

/**
 * execute_commands - Execute multiple commands separated by semicolons
 * @input: The user input containing commands
 */
void execute_commands(char *input)
{
	char *token;
	char *commands[MAX_COMMANDS];
	int i = 0;

	token = strtok(input, ";");

	while (token != NULL && i < MAX_COMMANDS)
	{
		commands[i++] = token;
		token = strtok(NULL, ";");
	}

	commands[i] = NULL;

	for (i = 0; commands[i] != NULL; i++)
	{
		char command[MAX_INPUT_SIZE];
		char *args[2];

		strcpy(command, commands[i]);

		parse_input(command, command, args);
		execute_command(command);
	}
}

/**
 * strip_comments - Remove comments from a command
 * @command: The command to remove comments from
 * Return: The command without comments
 */
char *strip_comments(char *command)
{
	char *result;
	int len = strlen(command) + 1;
	int i;

	result = malloc(len);
	if (result == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (i = 0; command[i] != '\0' && command[i] != '#'; i++)
	{
		result[i] = command[i];
	}

	result[i] = '\0';

	return result;
}

/**
 * execute_logical_commands - Execute commands with logical operators && and ||
 * @input: The user input containing commands
 */
void execute_logical_commands(char *input)
{
	/* Replace variables before executing commands */
	char *replaced_input = replace_variables(input);

	/* Remove comments before splitting into commands */
	char *comment_stripped_input = strip_comments(replaced_input)
		;
	char *token = strtok(comment_stripped_input, "&|");
	char *commands[MAX_COMMANDS];
	int i = 0;
		

	while (token != NULL && i < MAX_COMMANDS)
	{
		commands[i++] = token;
		token = strtok(NULL, "&|");
	}

	commands[i] = NULL;

	for (i = 0; commands[i] != NULL; i++)
	{
		char *replaced_command = replace_variables(commands[i]);

		char command[MAX_INPUT_SIZE];
		char *args[2];

		strcpy(command, replaced_command);
		free(replaced_command);

		parse_input(command, command, args);
		if (execute_command_with_logical(command) == -1)
			break; /* Stop executing commands if && fails */
	}

	free(replaced_input);
	free(comment_stripped_input);
}

/**
 * execute_command_with_logical - Execute a command with logical operators
 * @command: The command to execute
 * Return: 0 on success, -1 on failure
 */
int execute_command_with_logical(char *command)
{
	int status;
	pid_t pid;

	pid = fork();

	if (pid == -1)
	{
		perror("fork");
		return -1;
	}

	if (pid == 0)
	{
		/* Child process */
		char *args[2];
		parse_input(command, command, args);
		execve(command, args, NULL);
		perror("execve");
		_exit(EXIT_FAILURE);
	}
	else
	{
		/* Parent process */
		waitpid(pid, &status, 0);

		if (WIFEXITED(status))
		{
			if (WEXITSTATUS(status) != 0)
				return -1; /* Command failed */
		}
		else
		{
			perror("waitpid");
			return -1; /* waitpid failed */
		}
	}

	return 0;
}

/**
 * parse_input - Parse user input into a command and arguments
 * @input: The user input
 * @command: The resulting command
 * @args: Array to store arguments
 */
void parse_input(char *input, char *command, char *args[])
{
	char **tokens = tokenize_input(input);

	if (tokens[0] == NULL)
		return;

	strcpy(command, tokens[0]);

	args[0] = command;

	if (tokens[1] != NULL)
		args[1] = tokens[1];
	else
		args[1] = NULL;
}

/**
 * shell_prompt - Display the shell prompt
 */
void shell_prompt(void)
{
	write(1, "$ ", 2);
}

/**
 * find_command_path - Find the full path of the command using PATH
 * @command: The command to find
 * Return: The full path of the command or NULL if not found
 */
char *find_command_path(char *command)
{
	char *path = getenv("PATH");
	char *token, *path_copy, *full_path;
	struct stat st;

	if (path == NULL)
		return NULL;

	path_copy = strdup(path);

	if (path_copy == NULL)
	{
		perror("strdup");
		_exit(EXIT_FAILURE);
	}

	token = strtok(path_copy, PATH_DELIMITER);

	while (token != NULL)
	{
		full_path = malloc(strlen(token) + strlen(command) + 2);

		if (full_path == NULL)
		{
			perror("malloc");
			free(path_copy);
			_exit(EXIT_FAILURE);
		}

		sprintf(full_path, "%s/%s", token, command);

		if (stat(full_path, &st) == 0)
		{
			free(path_copy);
			return full_path;
		}

		free(full_path);
		token = strtok(NULL, PATH_DELIMITER);
	}

	free(path_copy);
	return NULL;
}

/**
 * change_directory - Change the current working directory
 * @path: The path to the new directory
 * Return: 0 on success, -1 on failure
 */
int change_directory(char *path)
{
	if (chdir(path) == -1)
	{
		perror("chdir");
		return -1;
	}

	return 0;
}

/**
 * set_env_variable - Set a new environment variable or modify an existing one
 * @name: The name of the environment variable
 * @value: The value to set
 * Return: 0 on success, -1 on failure
 */
int set_env_variable(char *name, char *value)
{
	int overwrite = 1; /* Set to overwrite the variable if it already exists */

	if (setenv(name, value, overwrite) == -1)
	{
		perror("setenv");
		return -1;
	}

	return 0;
}

/**
 * unset_env_variable - Remove an environment variable
 * @name: The name of the environment variable
 * Return: 0 on success, -1 on failure
 */
int unset_env_variable(char *name)
{
	if (unsetenv(name) == -1)
	{
		perror("unsetenv");
		return (-1);
	}

	return (0);
}

/**
 * shell_exit - Exit the shell
 */
void shell_exit(void)
{
	write(1, "Exit\n", 5);
	exit(EXIT_SUCCESS);
}

/**
 * shell_env - Print the current environment variables
 */
void shell_env(void)
{
	extern char **environ;
	int i = 0;

	while (environ[i] != NULL)
	{
		write(1, environ[i], strlen(environ[i]));
		write(1, "\n", 1);
		i++;
	}
}

/**
 * _getline - Read a line from the standard input
 * @lineptr: Pointer to the buffer containing the read line
 * @n: Number of bytes to read
 * Return: Number of bytes read
 */
ssize_t _getline(char **lineptr, size_t *n)
{
	char *buffer = NULL;
	size_t size = 0;
	ssize_t bytes_read;

	bytes_read = read(0, buffer, size);

	if (bytes_read == -1)
	{
		perror("read");
		_exit(EXIT_FAILURE);
	}

	buffer[bytes_read] = '\0';
	*lineptr = buffer;
	*n = size;

	return bytes_read;
}

/**
 * tokenize_input - Tokenize the input into an array of strings
 * @input: The input string
 * Return: Array of tokens
 */
char **tokenize_input(char *input)
{
	char *token;
	char **tokens = malloc(sizeof(char *) * 3);
	int i = 0;

	if (tokens == NULL)
	{
		perror("malloc");
		_exit(EXIT_FAILURE);
	}

	token = strtok(input, " ");

	while (token != NULL)
	{
		tokens[i++] = strdup(token);

		if (tokens[i - 1] == NULL)
		{
			perror("strdup");
			_exit(EXIT_FAILURE);
		}

		token = strtok(NULL, " ");
	}

	tokens[i] = NULL;

	return tokens;
}

/**
 * print_aliases - Print aliases
 */
void print_aliases(void)
{
	int i;

	for (i = 0; i < MAX_ALIASES; i++)
	{
		if (aliases[i].name != NULL && aliases[i].value != NULL)
			printf("%s='%s'\n", aliases[i].name, aliases[i].value);
	}
}

/**
 * set_alias - Set or modify an alias
 * @name: The name of the alias
 * @value: The value of the alias
 * Return: 0 on success, -1 on failure
 */
int set_alias(char *name, char *value)
{
	int i;

	for (i = 0; i < MAX_ALIASES; i++)
	{
		if (aliases[i].name == NULL)
		{
			aliases[i].name = strdup(name);
			if (aliases[i].name == NULL)
			{
				perror("strdup");
				return -1;
			}

			aliases[i].value = strdup(value);
			if (aliases[i].value == NULL)
			{
				perror("strdup");
				free(aliases[i].name);
				aliases[i].name = NULL;
				return -1;
			}

			return 0; /* Alias set successfully */
		}
		else if (strcmp(aliases[i].name, name) == 0)
		{
			free(aliases[i].value);
			aliases[i].value = strdup(value);
			if (aliases[i].value == NULL)
			{
				perror("strdup");
				return -1;
			}

			return 0; /* Alias modified successfully */
		}
	}

	return -1; /* No space for new alias */
}

/**
 * shell_alias - Handle the alias command
 * @input: The user input
 */
void shell_alias(char *input)
{
	char *token;
	char *name;
	char *value;

	token = strtok(input, "=");

	if (token == NULL)
	{
		print_aliases();
		return;
	}

	name = strdup(token);

	if (name == NULL)
	{
		perror("strdup");
		return;
	}

	token = strtok(NULL, "=");

	if (token == NULL)
	{
		free(name);
		return;
	}

	value = strdup(token);

	if (value == NULL)
	{
		perror("strdup");
		free(name);
		return;
	}

	set_alias(name, value);

	free(name);
	free(value);
}

/* Definition of global array to store aliases */
struct Alias aliases[MAX_ALIASES];

/**
 * ... (previous code remains unchanged)
 */

/**
 * replace_variables - Replace variables in a command
 * @command: The command to replace variables in
 * Return: The command with variables replaced
 */
char *replace_variables(char *command)
{
	char *result;
	char *temp;
	int len = strlen(command) + 1;
	int i;

	result = malloc(len);
	if (result == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (i = 0; command[i] != '\0'; i++)
	{
		if (command[i] == '$')
		{
			if (command[i + 1] == '$')
			{
				temp = malloc(len + 10);
				if (temp == NULL)
				{
					perror("malloc");
					exit(EXIT_FAILURE);
				}
				snprintf(temp, len + 10, "%s%d", result, getpid());
				free(result);
				result = temp;
				i++; /* Skip the second '$' */
			}
			else if (command[i + 1] == '?')
			{
				temp = malloc(len + 10);
				if (temp == NULL)
				{
					perror("malloc");
					exit(EXIT_FAILURE);
				}
				snprintf(temp, len + 10, "%s%d", result, 0); /* TODO: Replace 0 with actual exit status */
				free(result);
				result = temp;
				i++; /* Skip the '?' */
			}
			else
			{
				temp = malloc(len + 1);
				if (temp == NULL)
				{
					perror("malloc");
					exit(EXIT_FAILURE);
				}
				snprintf(temp, len + 1, "%s%c", result, command[i]);
				free(result);
				result = temp;
			}
		}
		else
		{
			temp = malloc(len + 1);
			if (temp == NULL)
			{
				perror("malloc");
				exit(EXIT_FAILURE);
			}
			snprintf(temp, len + 1, "%s%c", result, command[i]);
			free(result);
			result = temp;
		}
	}

	return result;
}

/**
 * execute_file_commands - Execute commands from a file
 * @filename: The name of the file containing commands
 */
void execute_file_commands(const char *filename)
{
	FILE *file = fopen(filename, "r");
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	if (file == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, file)) != -1)
	{
		if (read > 0 && line[read - 1] == '\n')
			line[read - 1] = '\0'; /* Remove the newline character */

		/* Execute the command */
		execute_command(line);
	}

	free(line);
	fclose(file);
}

/**
 * main - Entry point for the simple shell program
 * Return: Always 0
 */
int main(void)
{
	char *input = NULL;
	char command[MAX_INPUT_SIZE];
	char *args[2];
	size_t input_size;

	while (1)
	{
		shell_prompt();

		if (_getline(&input, &input_size) == 0)
		{
			write(1, "\n", 1);
			shell_exit();
		}

		input[strcspn(input, "\n")] = '\0';

		parse_input(input, command, args);

		if (strcmp(command, "exit") == 0)
			shell_exit();
		else if (strcmp(command, "env") == 0)
			shell_env();
		else if (strcmp(command, "setenv") == 0)
			set_env_variable(args[1], args[2]);
		else if (strcmp(command, "unsetenv") == 0)
			unset_env_variable(args[1]);
		else if (strcmp(command, "cd") == 0)
			change_directory(args[1]);
		else
			execute_command(command);
	}

	return 0;}
