#include "shell.h"

/**
 * main - Entry point for the simple shell program
 * Return: Always 0
 */
int main(void)
{
	char input[MAX_INPUT_SIZE];
	char command[MAX_INPUT_SIZE];
	char *args[2];

	while (1)
	{
		write(1, "$ ", 2);

		if (read(0, input, sizeof(input)) == 0)
		{
			write(1, "\n", 1);
			break;
		}

		input[strcspn(input, "\n")] = '\0';

		parse_input(input, command, args);
		execute_command(command);
	}

	return 0;
}
