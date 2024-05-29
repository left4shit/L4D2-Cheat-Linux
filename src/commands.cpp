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
  if (strstr(args[1], "on")) {
    ptraceWrite(game->pid, game->ClientModule + game->offsets.GlowAddr, "\x0F\x84\x2F\xFE\xFF\xFF\x0F\xB6", 8);
    ptraceWrite(game->pid, game->ClientModule + game->offsets.InfectedGlowAddr, "\x75\xB6\x90\x90\x90\x90\xA1\xDC", 8);
    puts("Turned on esp");
  } else {
    ptraceWrite(game->pid, game->ClientModule + game->offsets.GlowAddr, "\x0F\x84\x7E\xFC\xFF\xFF\x0F\xB6", 8);
    ptraceWrite(game->pid, game->ClientModule + game->offsets.InfectedGlowAddr, "\x0F\x84\xE4\x01\x00\x00\xA1\xDC", 8);
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

void toggleNoSpread(Game* game, char ** args) {
  if (strstr(args[1], "on")) {
    ptraceWrite(game->pid, game->ClientModule + game->offsets.NoSpread, "\xC7\x86\xF0\x0C\x00\x00\x00\x00\x00\x00\x90\x8B", 12);
    puts("Turned on no-spread");
  } else {
    ptraceWrite(game->pid, game->ClientModule + game->offsets.NoSpread, "\xF3\x0F\x11\x86\xF0\x0C\x00\x00\x8B\x78\x30\x8B", 12);
    puts("Turned off no-spread");
  }
}
