#ifndef HISTORY_H
#define HISTORY_H

#include "defs.h"

void initialize_history (void);

char * get_history_down (void);
char * get_history_up (void);

void push_cmd (char *command);

int history_cmd (int n);
char *get_line_position(int position);
#endif  // HISTORY_H