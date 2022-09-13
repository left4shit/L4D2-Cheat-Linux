/* keyboard.c
 *
 * Licensed under GPL version 3 or later.
 * See LICENSE for copyright information.
 */

#include "keyboard.h"
#include "utils.h"

#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int keyfd;
static int uinputfd;
static Key KeyList[144];

char checkKey(int key) { return KeyList[key].Value == 1; }

/* Finds a /dev/input device */
void openKeyboard(void) {
  char eventName[9], lineBuf[1024];
  char fileName[FILENAME_MAX] = "/dev/input/";
  char *ptr;
  char *ptr2;
  FILE *f;

  if ((f = fopen("/proc/bus/input/devices", "r")) == NULL)
    die("Could not open /proc/bus/input/devices");

  while (fgets(lineBuf, sizeof(lineBuf), f)) {
    if ((ptr = strstr(lineBuf, "Handlers="))) {
      ptr += strlen("Handlers=");
      if ((ptr = strstr(ptr, "event"))) {
        if ((ptr2 = strchr(ptr, ' ')))
          *ptr2 = '\0';
        sprintf(eventName, "%s", ptr);
      }
    }
    if (strstr(lineBuf, "EV=120013"))
      break;
  }

  fclose(f);

  strcat(fileName, eventName);
  if ((keyfd = open(fileName, O_RDONLY | O_NONBLOCK)) == -1)
    die("Could not open the input device");
}

/* Creates a virtual device */
/* @TODO Support Alpine Linux */
void openUinputKeyboard(void) {
  struct uinput_setup usetup = {0};

  if ((uinputfd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) == -1)
    die("Could not open /dev/uinput");

  ioctl(uinputfd, UI_SET_EVBIT, EV_KEY);
  ioctl(uinputfd, UI_SET_KEYBIT, KEY_SPACE);

  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = 0x046d;
  usetup.id.product = 0xc53d;

  strcpy(usetup.name, "Logitech USB Receiver");

  ioctl(uinputfd, UI_DEV_SETUP, &usetup);
  ioctl(uinputfd, UI_DEV_CREATE);
}

/* Handles KEYPRESS and KEYRELEASE events */
void manageInput(void) {
  struct input_event ie;
  int bytes;

  if ((bytes = read(keyfd, &ie, sizeof(ie))) < 1 && ie.type != EV_KEY)
    return;

  if (KeyList[ie.code].exists == 0) {
    Key key;
    key.exists = 1;
    key.Value = ie.value;
    KeyList[ie.code] = key;
    return;
  }

  KeyList[ie.code].Value = ie.value;
}

/* Send a keyboard event with a specified type */
static int sendEvent(int type, int code, int val) {
  struct input_event ie;

  ie.type = type;
  ie.code = code;
  ie.value = val;

  return write(uinputfd, &ie, sizeof(ie));
}

/* Send then release a key */
void sendInput(int keyCode) {
  sendEvent(EV_KEY, keyCode, 1);
  sendEvent(EV_SYN, SYN_REPORT, 0);
  sendEvent(EV_KEY, keyCode, 0);
  sendEvent(EV_SYN, SYN_REPORT, 0);
}
