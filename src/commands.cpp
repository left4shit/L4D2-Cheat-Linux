/* commands.c
 *
 * Licensed under GPL version 3 or later.
 * See LICENSE for copyright information.
 */

#include "commands.h"
#include "mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Handle the bhop hotkey */
void toggleBhop(Game * game, char c) { game->options.Bhop = c; }

/* Handle the bhopdelay command */
void bhopDelay(Game * game, char ** args) {
  if (strstr(args[1], "on")) {
    game->options.BhopDelay = 1;
    puts("Turned on bhop delay");
  } else {
    game->options.BhopDelay = 0;
    puts("Turned off bhop delay");
  }
}

/* Handle the glow command */
void toggleEsp(Game *game, char ** args) {
  const char *NOP = "\x90\x90\x90\x90\x90\x90\xFF\x90";
  const char *REV = "\x0F\x83\xD1\x01\x00\x00\xFF\x90";

  if (strstr(args[1], "on")) {
    ptraceWrite(game->pid, game->ClientModule + game->offsets.GlowAddr, "\x90\x90\x90\x90\x90\x90\xFF\x90", 8);
    ptraceWrite(game->pid, game->ClientModule + game->offsets.InfectedGlowAddr, "\x90\x90\x90\x90\x90\x90\xA1\x7C", 8);
    puts("Turned on esp");
  } else {
    ptraceWrite(game->pid, game->ClientModule + game->offsets.GlowAddr, "\x0F\x83\xD1\x01\x00\x00\xFF\x90", 8);
    ptraceWrite(game->pid, game->ClientModule + game->offsets.InfectedGlowAddr, "\x0F\x83\xBD\x00\x00\x00\xA1\x7C", 8);
    puts("Turned off esp");
  }
}

/* Handle the noboom command */
void toggleNoBoom(Game *game, char ** args) {
  const char *NOP = "\x90\x90\x90\x90\x90\x89\x1C\x24";
  const char *REV = "\xE8\x36\x2B\xFF\xFF\x89\x1C\x24";

  if (strstr(args[1], "on")) {
    ptraceWrite(game->pid, game->ClientModule + game->offsets.BoomAddr, NOP, 8);
    puts("Turned on no-boom");
  } else {
    ptraceWrite(game->pid, game->ClientModule + game->offsets.BoomAddr, REV, 8);
    puts("Turned off no-boom");
  }
}
