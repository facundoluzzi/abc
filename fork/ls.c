#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>

// Busca a un usuario por UID
char *uid(uid_t uid);

// A partir de un archivo en ei directorio actual imprime los permisos, usuario
// y nombre. En caso de ser un enlace simbolico, indica a que archivo/directorio apunta el mismo.
void info(char *file);

char *
uid(uid_t uid)
{
	return getpwuid(uid)->pw_name;
}

void
info(char *file)
{
	struct stat st;
	stat(file, &st);
	if (lstat(file, &st) < 0) {
		perror("Error examinando con lstat");
		exit(-1);
	}
	char mod[11] = "----------";
	if (S_ISDIR(st.st_mode)) {
		mod[0] = 'd';
	}
	if (S_ISLNK(st.st_mode)) {
		mod[0] = 'l';
		char *linkname = malloc(st.st_size + 1);
		if (linkname == NULL) {
			perror("Error en malloc");
			exit(-1);
		}

		ssize_t r = readlink(file, linkname, st.st_size + 1);
		if (r == -1) {
			perror("Error en readlink");
			exit(-1);
		}

		if (r > st.st_size) {
			perror("Error en enlace simbolico");
			exit(-1);
		}

		linkname[r] = '\0';
		strcat(file, " -> ");
		strcat(file, linkname);

		free(linkname);
	}
	if (st.st_mode & S_IRUSR)
		mod[1] = 'r';
	if (st.st_mode & S_IWUSR)
		mod[2] = 'w';
	if (st.st_mode & S_IXUSR)
		mod[3] = 'x';
	if (st.st_mode & S_IRGRP)
		mod[4] = 'r';
	if (st.st_mode & S_IWGRP)
		mod[5] = 'w';
	if (st.st_mode & S_IXGRP)
		mod[6] = 'x';
	if (st.st_mode & S_IROTH)
		mod[7] = 'r';
	if (st.st_mode & S_IWOTH)
		mod[8] = 'w';
	if (st.st_mode & S_IXOTH)
		mod[9] = 'x';
	printf("%s  %s  %s \n", mod, uid(st.st_uid), file);
}

int
main(int argc, char *argv[])
{
	(void) argc;
	struct dirent *entry;
	DIR *directory = opendir(".");
	if (directory == NULL) {
		perror("Error con opendir");
		exit(-1);
	}
	while (entry = readdir(directory)) {
		info(entry->d_name);
	}

	closedir(directory);
	return 0;
}