/* hander.h
 *
 * Licensed under GPL version 3 or later.
 * See LICENSE for copyright information.
 */

#ifndef _HANDLER_H
#define _HANDLER_H

#include "game.h"

/* Func definition of a command */
typedef void (*cmdfunc)(Game *, char **);
typedef struct {
  const char *name; /* Name of a command */
  cmdfunc func;     /* Func of a command */
} Command;

typedef void (*togglefunc)(Game *, char);
typedef struct {
  int Key;         /* Key that toggles a func */
  char Value;      /* Bool, what to set the value to (@TODO: Dynamic type) */
  togglefunc func; /* Function that handles Value */
} Toggle;

/* Checks & runs a command with the split arguments */
void executeCommand(Game *, char *);
/* Handle key presses to make hotkeys */
void handleInput(Game *, int);
/* Split stdin into a command + arguments */
char **splitArguments(char *);

#endif /* _HANDLER_H */
