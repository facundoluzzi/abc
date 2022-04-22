#include "builtin.h"

// returns true if the 'exit' call
// should be performed
//
// (It must not be called from here)
int
exit_shell(char *cmd)
{
	if (strcmp(cmd, "exit") == 0) {
		exit(0);
	}
	return 0;
}

// returns true if "chdir" was performed
//  this means that if 'cmd' contains:
// 	1. $ cd directory (change to 'directory')
// 	2. $ cd (change to $HOME)
//  it has to be executed and then return true
//
//  Remember to update the 'prompt' with the
//  	new directory.
//
// Examples:
//  1. cmd = ['c','d', ' ', '/', 'b', 'i', 'n', '\0']
//  2. cmd = ['c','d', '\0']
int
cd(char *cmd)
{
	if (strlen(cmd) > 1) {
		bool is_cd = cmd[0] == 'c' && cmd[1] == 'd';
		int n;
		if (is_cd) {
			if (strlen(cmd) > 3) {
				char *path = &cmd[3];
				n = chdir(path);
				if (n == 0) {
					if (getcwd(promt, (size_t) BUFLEN) ==
					    NULL) {
						perror("Error en actualizacion "
						       "del promt");
						exit(1);
					}
					return 1;
				} else {
					return 1;
				}
			} else {
				char *dir_home = getenv("HOME");
				n = chdir(dir_home);
				if (n == 0) {
					if (getcwd(promt, (size_t) BUFLEN) ==
					    NULL) {
						perror("Error en actualizacion "
						       "del "
						       "promt");
						exit(1);
					}
					return 1;
				} else {
					return 1;
				}
			}
		}
	}
	return 0;
}

// returns true if 'pwd' was invoked
// in the command line
//
// (It has to be executed here and then
// 	return true)
int
pwd(char *cmd)
{
	if (strcmp(cmd, "pwd") == 0) {
		if (getcwd(promt, (size_t) BUFLEN) == NULL) {
			perror("Error en actualizacion del promt");
			exit(1);
		};
		printf("%s\n", promt);
		return 1;
	}
	return 0;
}
