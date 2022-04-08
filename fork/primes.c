#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

void recursive(int file_descriptor, int number);

void
recursive(int file_descriptor, int number)
{
	int recv;
	int first_message;
	if (read(file_descriptor, &first_message, sizeof(first_message)) < 0) {
		perror("[P] Error en Read");
		_exit(-1);
	}
	if (first_message < number && first_message > 0) {
		printf("primo %d\n", first_message);
		int fds_on_recursivity[2];
		int v = pipe(fds_on_recursivity);
		if (v < 0) {
			_exit(-1);
		}
		int i = fork();

		if (i == 0) {
			close(file_descriptor);
			close(fds_on_recursivity[1]);
			recursive(fds_on_recursivity[0], number);
			close(fds_on_recursivity[0]);
		} else {
			close(fds_on_recursivity[0]);
			while (read(file_descriptor, &recv, sizeof(recv))) {
				if (recv % first_message != 0) {
					if (write(fds_on_recursivity[1],
					          &recv,
					          sizeof(recv)) < 0) {
						perror("Error en Write\n");
						_exit(-1);
					}
				}
			}
			close(fds_on_recursivity[1]);
			close(file_descriptor);
			wait(NULL);
		}
	}
}


int
main(int argc, char **argv)
{
	(void) argc;
	int number = atoi(argv[1]);
	int file1[2];
	int v = pipe(file1);
	if (v < 0) {
		_exit(-1);
	}
	int fork1 = fork();

	if (fork1 == 0) {
		close(file1[1]);
		recursive(file1[0], number);
		close(file1[0]);
	}
	if (fork1 > 0) {
		close(file1[0]);
		int n = 2;
		while (n <= number) {
			if (write(file1[1], &n, sizeof(n)) < 0) {
				perror("Error en Write");
				_exit(-1);
			}
			n++;
		}
		close(file1[1]);
		wait(NULL);
	}

	return 0;
}
