#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("El formato para invocar a cp debe ser cp 'archivo1' "
		       "'archivo2'");
		exit(-1);
	}
	struct stat sb;
	if (lstat(argv[1], &sb) < 0) {
		perror("Error examinando con lstat");
		exit(-1);
	}
	if (!S_ISREG(sb.st_mode)) {
		printf("El archivo a copiar debe ser un archivo regular");
		exit(-1);
	}
	// Abrimos el archivo fuente
	int fds_src = open(argv[1], O_RDONLY);
	if (fds_src < 0) {
		perror("Error abriendo el archivo fuente");
		exit(-1);
	}
	// Abrimos el archivo destino, combinando el flag O_CREAT con O_EXCL para
	// asegurarnos de crear el archivo. Si este existe, entonces la llamada a open fallara
	int fds_dst = open(argv[2], O_RDWR | O_CREAT | O_EXCL, sb.st_mode);
	if (fds_dst < 0) {
		perror("Error creando el archivo destino");
		close(fds_src);
		exit(-1);
	}
	// Truncamos el tamaño del archivo de destino, para realizar una copia exacta en tamaño del archivo fuente
	if (ftruncate(fds_dst, sb.st_size) < 0) {
		perror("Error trucando el tamaño el archivo destino");
		close(fds_src);
		close(fds_dst);
		exit(-1);
	}

	// Mapeamos a memoria el archivo fuente, y el archivo destino
	void *msrc = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fds_src, 0);
	void *mdst = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, fds_dst, 0);
	if ((msrc == MAP_FAILED) || (mdst == MAP_FAILED)) {
		perror("Error al mapear los archivos a memoria");
		close(fds_src);
		close(fds_dst);
		exit(-1);
	}
	// Copiamos los datos en memoria del archivo fuente, al archivo destino
	memcpy(mdst, msrc, sb.st_size);

	close(fds_src);
	close(fds_dst);
	return 0;
}