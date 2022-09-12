#include <stdio.h>
#include "defs.h"
#include "utils.h"
#include "history.h"

static FILE *history_file = NULL;
static char history_file_dir[PRMTLEN] = { 0 };
static ssize_t actual = 0;
static int lines = 0;
char ret[1] = { 0 };

// Configuraciones iniciales para el comando History
void
initialize_history()
{
	// De estar definida la variable de entorno HISTFILE, la misma contendrá
	// la ruta al archivo con los comandos ejecutados en el pasado. En caso
	// contrario, utilizar como ruta por omisión a ~/.fisop_history.
	if (getenv("HISTFILE")) {
		strcpy(history_file_dir, getenv("HISTFILE"));
	} else {
		strcpy(history_file_dir, getenv("HOME"));
		strcat(history_file_dir, "/.fisop_history");
	}
	history_file = fopen(history_file_dir, "r");

	char *line;
	size_t len = 0;

	// Se establecen la cantidad de lineas (comandos) por las que vamos a poder navegar
	int count = 0;
	if (history_file) {
		while (getline(&line, &len, history_file) != -1) {
			count++;
		}
		fclose(history_file);
	}
	free(line);
	lines = count;

	return;
}

// Navega, y devuelve el comando anterior al actual
char *
get_history_up()
{
	if (!(actual > lines)) {
		actual++;
	}
	return get_line_position(actual);
}

// Navega, y devuelve el comando posterior al actual
char *
get_history_down()
{
	if (actual <= 1) {
		return ret;
	}
	actual--;
	return get_line_position(actual);
}

// Se agrega un comando al history file, archivo el cual contiene una lista con cada comando que es llamado
void
push_cmd(char *command)
{
	if (!command) {
		return;
	}
	actual = 0;
	history_file = fopen(history_file_dir, "a");
	lines++;
	fprintf_debug(history_file, "%s\n", command);
	fclose(history_file);
	return;
}

// Lista todos los comandos llamados anteriormente
int
history_cmd(int n)
{
	history_file = fopen(history_file_dir, "r");
	if (!history_file) {
		return -1;
	}

	char *line;
	size_t len = 0;

	if (n != 0) {
		int lines_count = 0;
		while (getline(&line, &len, history_file) != -1)
			lines_count++;

		fseek(history_file, 0, SEEK_SET);

		if (lines_count > n)
			for (int i = 0; i < lines_count - n; i++) {
				int u __attribute__((unused)) =
				        getline(&line, &len, history_file);
			}
	}
	while (getline(&line, &len, history_file) != -1) {
		printf_debug("%s", line);
	}
	free(line);
	fclose(history_file);
	return 0;
}

// Devuelve un comando en determinada posicion en el archivo que contiene una lista de comandos
char *
get_line_position(int position)
{
	history_file = fopen(history_file_dir, "r");
	if (!history_file) {
		return ret;
	}
	char *line;
	size_t len = 0;
	int lines_count = 0;
	while (getline(&line, &len, history_file) != -1) {
		lines_count++;
	}
	fseek(history_file, 0, SEEK_SET);
	for (int i = 0; i < lines_count - position; i++) {
		int u __attribute__((unused)) =
		        getline(&line, &len, history_file);
	}
	int u __attribute__((unused)) = getline(&line, &len, history_file);
	line[strcspn(line, "\n")] = 0;
	fclose(history_file);

	return line;
}