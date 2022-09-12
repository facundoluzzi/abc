#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

int
main(void)
{
	int file_descriptor_one[2];
	int r = pipe(file_descriptor_one);
	if (r < 0) {
		perror("Error en pipe!\n");
		_exit(-1);
	}

	int file_descriptor_two[2];
	int w = pipe(file_descriptor_two);
	if (w < 0) {
		perror("Error en pipe!\n");
		_exit(-1);
	}

	printf("Hola, soy PID %d\n\n", getpid());
	printf("- primer pipe me devuelve: [%d, %d] \n",
	       file_descriptor_one[0],
	       file_descriptor_one[1]);
	printf("- segundo pipe me devuelve: [%d, %d] \n",
	       file_descriptor_two[0],
	       file_descriptor_two[1]);

	int i = fork();

	if (i < 0) {
		printf("Error en fork!%d\n", i);
		_exit(-1);
	}
	if (i == 0) {
		printf("\nDonde fork me devuelve %d:\n", i);
		printf("- getpid me devuelve: %d\n", getpid());
		printf("- getppid me devuelve: %d\n", getppid());
		close(file_descriptor_one[1]);
		close(file_descriptor_two[0]);
		int recv = 0;
		if (read(file_descriptor_one[0], &recv, sizeof(recv)) < 0) {
			perror("[H] Error en Read");
			_exit(-1);
		}
		printf("- recibo valor: %d via fd=%d\n",
		       recv,
		       file_descriptor_one[0]);

		close(file_descriptor_one[0]);
		printf("- reenvio valor en fd=%d y termino\n",
		       file_descriptor_two[1]);
		if (write(file_descriptor_two[1], &recv, sizeof(recv)) < 0) {
			perror("[H] Error en Write");
			_exit(-1);
		}
		close(file_descriptor_two[1]);
	} else {
		srand(time(NULL));
		int num = rand();

		printf("\nDonde fork me devuelve %d:\n", i);
		printf("- getpid me devuelve: %d\n", getpid());
		printf("- getppid me devuelve: %d\n", getppid());
		printf("- random me devuelve: %d\n", num);
		printf("- envio valor %d a traves de fd=%d\n",
		       num,
		       file_descriptor_one[1]);

		close(file_descriptor_one[0]);
		close(file_descriptor_two[1]);

		if (write(file_descriptor_one[1], &num, sizeof(num)) < 0) {
			perror("[P] Error en Write");
			_exit(-1);
		}

		close(file_descriptor_one[1]);
		int recv2 = 0;
		if (read(file_descriptor_two[0], &recv2, sizeof(recv2)) < 0) {
			perror("[P] Error en Read");
			_exit(-1);
		}
		printf("\nHola, de nuevo PID %d:\n", getpid());
		printf("- recibi valor: %d via fd=%d\n",
		       recv2,
		       file_descriptor_two[0]);

		close(file_descriptor_two[0]);
	}
	return 0;
}