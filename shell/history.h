#ifndef HISTORY_H
#define HISTORY_H

#include "defs.h"

void exit_history ();
void initialize_history ();

char * get_with_down_key ();
char * get_with_up_key ();

void push_cmd (char *command);

int history_cmd ();

#endif  // HISTORY_H