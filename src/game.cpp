/* game.c
 *
 * Licensed under GPL version 3 or later.
 * See LICENSE for copyright information.
 */

#include "game.h"
#include "mem.h"
#include "utils.h"

#include <stdio.h>
#include <unistd.h>
#include <string>

#ifndef VERSION
#define VERSION "UNDEFINED"
#endif

/* Check if the game is running */
char checkGame(pid_t pid) {
  char fName[FILENAME_MAX];

  sprintf(fName, "/proc/%d/maps", pid);
  return access(fName, R_OK);
}

/* Initalizes the cheat */
void openGame(Game *game, char *name) {
  if ((game->pid = findPid(name)) == -1)
    die(std::string("Game not found").data());

  moduleAddr(game->pid, std::string("/bin/client.so").data(), &game->ClientModule,
             &game->ClientModuleEnd, 1);
  moduleAddr(game->pid, std::string("/bin/engine.so").data(), &game->EngineModule,
             &game->EngineModuleEnd, 1);

  if ((game->ClientModule + game->ClientModuleEnd) == 0)
    die(std::string("Could not get the client.so module\n").data());
  else if ((game->EngineModule + game->ClientModuleEnd) == 0)
    die(std::string("Could not get the engine.so module\n").data());

  printf("Version (%s) of the cheat has loaded\n"
         "%s -> PID: %d\n"
         "client.so -> (%x-%x)\n"
         "engine.so -> (%x-%x)\n",
         VERSION, name, game->pid, game->ClientModule, game->ClientModuleEnd,
         game->EngineModule, game->EngineModuleEnd);

  /* @TODO: replace with sigs */
  game->offsets.PlayerAddr = 0x00EDB3E8; /* LPlr Addr | (c2ce13e8 | xx?x) */
  game->offsets.GlowAddr = 0x7EE069;     /* Glow Addr */
  game->offsets.BoomAddr = 0x7F3E75;     /* Boom Addr */
  game->offsets.InfectedGlowAddr = 0x7EDFF5;
}

/* Checks if the LocalPlayer exists */
char playerFound(Game *game) {
  if (readAddr(game->pid, game->ClientModule + game->offsets.PlayerAddr,
               &game->player.Address, sizeof(game->player.Address)) == -1)
    return -1;

  return readAddr(game->pid, game->player.Address, &game->player,
                  sizeof(game->player) - sizeof(game->player.Address));
}