#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#define _GNU_SOURCE

int pidNumber(char s[256]);
void print_process(char pid[256]);

int
main()
{
	DIR *directory = opendir("/proc");
	if (directory == NULL) {
		perror("Error con opendir");
		exit(-1);
	}
	struct dirent *entry = readdir(directory);
	while (entry != NULL) {
		if (entry->d_type == DT_DIR && pidNumber(entry->d_name)) {
			print_process(entry->d_name);
		}
		entry = readdir(directory);
	}

	closedir(directory);
	return 0;
}

int
pidNumber(char s[256])
{
	for (int i = 0; s[i] != '\0'; i++) {
		if (isdigit(s[i]) == 0)
			return 0;
	}
	return 1;
}

void
print_process(char pid[256])
{
	char path[256] = "/proc/";
	strcat(path, pid);
	strcat(path, "/comm");
	FILE *file = fopen(path, "r");
	if (file) {
		char *line;
		size_t len = 0;
		if (getline(&line, &len, file) > 0) {
			printf("%s %s", pid, line);
		}
		fclose(file);
		free(line);
	} else {
		perror("Error al abrir /proc/PID/comm");
	}
}