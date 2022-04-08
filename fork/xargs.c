#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#ifndef NARGS
#define NARGS 4
#endif

void exec(char *command, char *args[]);

void
exec(char *command, char *args[])
{
	int i = fork();
	if (i == 0) {
		if (execvp(command, args) < 0) {
			printf("***Error: EXCEVP failed\n");
			exit(1);
		}
	} else {
		int wstatus;
		wait(&wstatus);
	}
}

void clean(char *args[]);

void
clean(char *args[])
{
	for (int i = 1; i < (NARGS + 2); i++) {
		args[i] = NULL;
	}
}

int
main(int argc, char *argv[])
{
	(void) argc;
	char *command = argv[1];
	char *line = NULL;
	size_t len = 0;

	char *args[NARGS + 2];
	args[0] = argv[1];
	clean(args);
	int count = 1;

	while (getline(&line, &len, stdin) != -1) {
		if (count <= NARGS) {
			line[strlen(line) - 1] = '\0';
			args[count] = strdup(line);
			count++;
		} else {
			exec(command, args);
			clean(args);
			line[strlen(line) - 1] = '\0';
			args[1] = strdup(line);
			count = 2;
		}
		line = NULL;
		len = 0;
	}
	if (count > 1) {
		exec(command, args);
	}
	free(line);
	return 0;
}
