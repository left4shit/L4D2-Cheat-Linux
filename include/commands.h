/* commands.h
 *
 * Licensed under GPL version 3 or later.
 * See LICENSE for copyright information.
 */

#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "game.h"

/* Handle the bhop hotkey */
void toggleBhop(Game *, char);
/* Handle the bhopdelay command */
void bhopDelay(Game *, char **);
/* Handle the glow command */
void toggleEsp(Game *, char **);
/* Handle the noboom command */
void toggleNoBoom(Game *, char **);
void toggleNoSpread(Game *, char **);

#endif /* _COMMANDS_H */
