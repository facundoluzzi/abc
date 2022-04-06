#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#define _GNU_SOURCE


char *strstr(const char *haystack, const char *needle);

char *strcasestr(const char *haystack, const char *needle);

void validate(char *haystack,
              char *last_path,
              char *needle,
              char *function(const char *a, const char *b));

void recursive(DIR *directory,
               char *subword,
               char *last_path,
               char *function(const char *a, const char *b));

void
validate(char *haystack,
         char *last_path,
         char *needle,
         char *function(const char *a, const char *b))
{
	if (function(last_path, needle) != NULL) {
		printf("%s\n", haystack);
	}
}

void
recursive(DIR *directory,
          char *subword,
          char *last_path,
          char *function(const char *, const char *))
{
	struct dirent *entry;
	while ((entry = readdir(directory)) != NULL) {
		int fds = dirfd(directory);
		int direct = openat(fds, entry->d_name, __O_DIRECTORY);
		DIR *new_directory = fdopendir(direct);
		char actually_path[PATH_MAX];
		strcpy(actually_path, last_path);
		strcat(actually_path, entry->d_name);
		if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") &&
		    strcmp(entry->d_name, "..")) {
			validate(actually_path, entry->d_name, subword, function);
			strcat(actually_path, "/");
			recursive(new_directory, subword, actually_path, function);
		}
		if (entry->d_type == DT_REG) {
			validate(actually_path, entry->d_name, subword, function);
		}
	}
	closedir(directory);
}

int
main(int argc, char *argv[])
{
	(void) argc;
	DIR *directory = opendir(".");
	if (directory == NULL) {
		perror("Error con opendir");
		exit(-1);
	}
	char *initial_path = "";
	char *subword = argv[1];
	if (strcmp(subword, "-i") == 0) {
		subword = argv[2];
		recursive(directory, subword, initial_path, strcasestr);
	} else {
		recursive(directory, subword, initial_path, strstr);
	}

	return 0;
}