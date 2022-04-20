#include "exec.h"

// sets "key" with the key part of "arg"
// and null-terminates it
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  key = "KEY"
//
static void
get_environ_key(char *arg, char *key)
{
	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}

// sets "value" with the value part of "arg"
// and null-terminates it
// "idx" should be the index in "arg" where "=" char
// resides
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  value = "value"
//
static void
get_environ_value(char *arg, char *value, int idx)
{
	size_t i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables received
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void
set_environ_vars(char **eargv, int eargc)
{
	int n = 0;
	while (n < eargc){
		char* to_expand = eargv[n];
		char* value = split_line(to_expand, '=');
		setenv(to_expand, value, 1);
		n++;
	}
}

// opens the file in which the stdin/stdout/stderr
// flow will be redirected, and returns
// the file descriptor
//
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file	
static int
open_redir_fd(char *file, int flags)
{
	// O_CREAT toma el valor binario 0100,
	// utilizando el operador logico & podemos verificar si tenemos
	// activado unicamente el byte 0100
	int verify_o_creat_flag = flags & 0100;
	if (verify_o_creat_flag == O_CREAT) {
		int file_descriptor = open(file, flags, S_IWUSR | S_IRUSR);
		return file_descriptor;
	}
	int file_descriptor = open(file, flags);
	return file_descriptor;
}

// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
// - casting could be a good option
void
exec_cmd(struct cmd *cmd)
{
	// To be used in the different cases
	struct execcmd *e;
	struct backcmd *b;
	struct execcmd *r;
	struct pipecmd *p;

	switch (cmd->type) {
	case EXEC: {
		// spawns a command
		//
		e = (struct execcmd *) cmd;
		set_environ_vars(e->eargv, e->eargc);
		char **args = e->argv;
		char *command = e->argv[0];
		if (execvp(command, args) < 0) {
			printf("Error en la ejecucion del comando (EXECVP)\n");
			exit(-1);
		}

		_exit(-1);
		break;
	}
	case BACK: {
		// runs a command in background
		//
		e = (struct execcmd *) cmd;
		b = (struct backcmd *) cmd;
		char **args = e->argv;
		char *command = e->argv[0];
		pid_t pid = b->pid;
		int wstatus;

		int i = fork();

		if (i == 0) {
			if (execvp(command, args) < 0) {
				printf("Error en la ejecucion del comando (EXECVP)\n");
				exit(-1);
			}
		} else {
			if ((pid = waitpid(pid, &wstatus, WNOHANG)) == -1) {
				perror("Error en WAITPID");
				exit(-1);
			}
			exit(0);
		}

		_exit(-1);
		break;
	}

	case REDIR: {
		// changes the input/output/stderr flow
		//
		// To check if a redirection has to be performed
		// verify if file name's length (in the execcmd struct)
		// is greater than zero
		//
		r = (struct execcmd *) cmd;
		int fds[3];
		char **args = r->argv;
		char *command = r->argv[0];

		if (strlen(r->in_file) > 0) {
			fds[0] = open_redir_fd(r->in_file, O_RDWR);
			if (dup2(fds[0], STDIN) == -1){
				exit(1);
			}
			close(fds[0]);
		}
		if (strlen(r->out_file) > 0) {
			fds[1] = open_redir_fd(r->out_file,
			                       O_RDWR | O_CREAT | O_TRUNC);
			if (dup2(fds[1], STDOUT) == -1){
				exit(1);
			}
			close(fds[1]);
		}

		if (strlen(r->err_file) > 0) {
			if (strlen(r->out_file) > 0){
				if (dup2(STDOUT, STDERR) == -1){
					exit(1);
				}
			} else{
				fds[2] = open_redir_fd(r->err_file,
									O_RDWR | O_CREAT | O_TRUNC);
				if (dup2(fds[2], STDERR) == -1){
					exit(1);
				}
				close(fds[2]);
			}
		}

		if (execvp(command, args) < 0) {
			printf("Error en la ejecucion del comando (EXECVP)\n");
			exit(-1);
		}

		_exit(-1);
		break;
	}

	case PIPE: {
		p = (struct pipecmd *) cmd;

		int file_descriptor[2];
		int v = pipe(file_descriptor);
		if (v < 0) {
			_exit(-1);
		}
		
		int i = fork();
		if (i < 0){
			perror("Error en FORK");
			exit(-1);
		}
		if (i == 0) {
			close(file_descriptor[0]);
			dup2(file_descriptor[1], 1);
			close(file_descriptor[1]);
			exec_cmd(p->leftcmd);
			exit(-1);
		} 

		int m = fork();
		if (m < 0){
			perror("Error en FORK");
			exit(-1);
		}
		if (m == 0){
			close(file_descriptor[1]);
			dup2(file_descriptor[0], 0);
			close(file_descriptor[0]);
			exec_cmd(p->rightcmd);
			exit(-1);
		}

		close(file_descriptor[1]);
		close(file_descriptor[0]);
		wait(NULL);
		wait(NULL);

		free_command(parsed_pipe);
		_exit(-1);
		break;
	}
	}
}