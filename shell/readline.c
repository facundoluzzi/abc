#include "defs.h"
#include "readline.h"
#include "utils.h"
#include "history.h"
#include <sys/ioctl.h>

static char buffer[BUFLEN];

void esc(int *i, size_t *row, size_t *col, int MAX_LENGTH);
void backspace(int *i, size_t *row, size_t *col, int MAX_LENGTH);
void writeChar(char c, size_t *row, size_t *col, int MAX_COL);
void writeWord(char *word, size_t *row, size_t *col, int MAX_COL);
void clearConsole(size_t *row, size_t *col);

// reads a line from the standard input
// and prints the prompt
char *
read_line(const char *promt)
{
	set_input_mode();

	int i = 0, c = 0;

	size_t col = 0, row = 0;
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int MAX_LENGTH = w.ws_col - 2;
	if (!MAX_LENGTH || MAX_LENGTH < 1) {
		MAX_LENGTH = 50;
	}
#ifndef SHELL_NO_INTERACTIVE
	fprintf(stdout, "%s %s %s\n", COLOR_RED, promt, COLOR_RESET);
	fprintf(stdout, "%s", "$ ");
	fflush(stdout);
#endif

	memset(buffer, 0, BUFLEN);

	while (1) {
		read(STDIN_FILENO, &c, 1);

		switch (c) {
		case EOF:
		case 4:
			return NULL;
		case 27:
			esc(&i, &row, &col, MAX_LENGTH);
			break;
		case 127:
			backspace(&i, &row, &col, MAX_LENGTH);
			break;
		case END_LINE:
			buffer[i] = END_STRING;
			putchar(END_LINE);
			push_cmd(buffer);
			return buffer;
		default:
			buffer[i] = c;
			i++;
			writeChar(c, &row, &col, MAX_LENGTH);
			break;
		}
#ifndef SHELL_NO_INTERACTIVE
		fflush(stdout);
#endif
	}
	return NULL;
}

void
esc(int *i, size_t *row, size_t *col, int MAX_LENGTH)
{
	int c = 0;
	read(STDIN_FILENO, &c, 1);
	switch (c) {
	case '[':
		read(STDIN_FILENO, &c, 1);
		switch (c) {
		case 'A':
			strcpy(buffer, get_history_up());
			break;
		case 'B':
			strcpy(buffer, get_history_down());
			break;
		default:
			return;
		}
		clearConsole(row, col);
		writeWord(buffer, row, col, MAX_LENGTH);
		*i = strlen(buffer);
		break;
	}
}

void
backspace(int *i, size_t *row, size_t *col, int MAX_LENGTH)
{
	if (*i <= 0)
		return;
	buffer[--(*i)] = 0;
	// Backspace para comandos con cualquier número de líneas
	if ((*col)-- == 0) {
		(*row)--;
		(*col) = MAX_LENGTH - 1;
		printf_debug("\b \b\b \b");
		printf_debug("\b \b\b \b\033[A\033[%iC", MAX_LENGTH + 2);
	}
	printf_debug("\b \b");
}

void
writeChar(char c, size_t *row, size_t *col, int MAX_COL)
{
	putchar(c);
	if (++(*col) >= (size_t) MAX_COL) {
		printf_debug("\n");
		*col = 0;
		(*row)++;
	}
}
void
writeWord(char *word, size_t *row, size_t *col, int MAX_COL)
{
	for (size_t i = 0; i < strlen(word); i++)
		writeChar(word[i], row, col, MAX_COL);
}

void
clearConsole(size_t *row, size_t *col)
{
	*col = 0;
	while (*row > 0) {
		(*row)--;
		printf_debug("\33[2K\r\033[A");
	};
	printf_debug("\33[2K\r$ ");
}
