#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include "noncanonic_mode.h"

/* Use this variable to remember original terminal attributes. */

struct termios saved_attributes;

void
reset_input_mode(void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

// Establece el input mode o modo de entrada como no canonico
// lo cual nos servira para leer caracteres simples a medida que estos son ingresados
void
set_input_mode(void)
{
  struct termios tattr;
  char *name;

  /* Make sure stdin is a terminal. */
  if (!isatty (STDIN_FILENO))
    {
      fprintf (stderr, "Not a terminal.\n");
      exit (EXIT_FAILURE);
    }

  /* Save the terminal attributes so we can restore them later. */
  tcgetattr (STDIN_FILENO, &saved_attributes);
  atexit (reset_input_mode);

  /* Set the funny terminal modes. */
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}