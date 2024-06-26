/* main.c
 *
 * Licensed under GPL version 3 or later.
 * See LICENSE for copyright information.
 */
#include "handler.h"
#include "keyboard.h"
#include "utils.h"
#include "MyDisplay.hpp"
#include "mem.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

static Game game;
static void *mainThread(void *);
static MyDisplay myDisplay;

int main(void)
{
  pthread_t threadID;
  char str[1024];

  if (getuid() != 0)
    die(std::string("You must run this program as root").data());

  openGame(&game, std::string("hl2_linux").data());
  pthread_create(&threadID, NULL, mainThread, NULL);

  while (1)
  {
    fputs("> ", stdout);

    if (fgets(str, 1024, stdin))
      executeCommand(&game, str);
  }
}

/* Main cheat thread */
static void *mainThread(void *_)
{
  (void)_; /* ignoring extra thread arg */
  int key, keyF, uinputF;

  /* For the randomized bhop delay */
  srand(time(NULL));

  if ((keyF = openKeyboard()) == -1)
    die(std::string("Could not open the input device").data());

  if ((uinputF = openUinputKeyboard()) == -1)
    die(std::string("Could not open /dev/uinput").data());

  while (checkGame(game.pid) != -1)
  {
    if (playerFound(&game) == -1)
      continue;

    if (myDisplay.keyDown("XK_space") &&
        (game.player.m_fFlags == 129 || game.player.m_fFlags == 641 ||
         game.player.m_fFlags == 131 || game.player.m_fFlags == 643))
    {

      // if (!game.options.BhopDelay || rand() % 50 <= 10) {
      //  Do the bhop faggot
      ptraceWrite(game.pid, game.ClientModule + game.offsets.ForceJump, "\x05", 1);
      usleep(100000);
      //}
    }
    else
    {
      ptraceWrite(game.pid, game.ClientModule + game.offsets.ForceJump, "\x04", 1);
    }

    // sleep(1);
  }

  die(std::string("Game not running").data());
  return NULL;
}
