/* mem.c
 *
 * Licensed under GPL version 3 or later.
 * See LICENSE for copyright information.
 */
//#define _GNU_SOURCE

#include "mem.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include <sys/ptrace.h>
#include <sys/wait.h>

/* Scan func (/proc/pid/maps) */
void moduleAddr(pid_t pid, char *lib, u_int32_t *start, u_int32_t *end,
                char perms) {
  char fName[FILENAME_MAX];
  char buf[248], lName[64], rwxp[5];
  FILE *f;

  sprintf(fName, "/proc/%d/maps", pid);
  if (lib)
    sprintf(lName, "%s", lib);

  if ((f = fopen(fName, "r")) == NULL)
    return;

  while (fgets(buf, sizeof(buf), f)) {
    if (lib && !strstr(buf, lName))
      continue;

    sscanf(buf, "%lx-%lx %s", (unsigned long *)start, (unsigned long *)end,
           rwxp);
    if (!perms || (rwxp[0] == 'r' && rwxp[2] == 'x'))
      break;
  }
  fclose(f);

  return;
}

/* Write func (ptrace), instructions */
void ptraceWrite(pid_t pid, u_int32_t addr, const char *buf, size_t size) {
  int i = 0, j = size / sizeof(long);
  pokeData data;

  ptrace(PTRACE_ATTACH, pid, 0, 0);
  waitpid(pid, 0, 0);

  for (i = 0; i < j; i++, buf += sizeof(long)) {
    memcpy(data.chars, buf, sizeof(long));
    ptrace(PTRACE_POKEDATA, pid, addr + i * 4, data.val);
  }

  if ((j = size % sizeof(long)) == 0) {
    ptrace(PTRACE_DETACH, pid, 0, 0);
    return;
  }

  memcpy(data.chars, buf, j);
  ptrace(PTRACE_POKEDATA, pid, addr + i * 4, data.val);

  ptrace(PTRACE_DETACH, pid, 0, 0);
  return;
}

/* Read func (/proc/pid/mem), values */
/* @TODO: replace lseek64 */
char readAddr(pid_t pid, u_int32_t addr, void *buf, size_t size) {
  int fd;
  int8_t ret;
  char fName[FILENAME_MAX];

  sprintf(fName, "/proc/%d/mem", pid);

  if ((fd = open(fName, O_RDONLY)) == -1)
    return -1;

  if ((ret = lseek64(fd, addr, SEEK_SET)) != -1)
    ret = read(fd, buf, size);

  close(fd);
  return ret;
}

/* Scan func (memory range), address */
u_int32_t ScanAddr(u_int32_t start, u_int32_t end, char *sig, char *mask,
                   size_t size) {
  size_t i;
  pokeData data;

  while (start < end) {
    data.val = start;
    for (i = 0; i < size; i++) {
      if (mask[i] != '?' && sig[i] != data.chars[i])
        break;
    }

    if (i == size)
      return start;

    start++;
  }

  return -1;
}

/* Write func (/proc/pid/mem), values */
/* @TODO: replace lseek64 */
char writeAddr(pid_t pid, u_int32_t addr, void *buf, size_t size) {
  int fd, ret;
  char fName[FILENAME_MAX];

  sprintf(fName, "/proc/%d/mem", pid);

  if ((fd = open(fName, O_WRONLY)) == -1)
    return -1;

  if ((ret = lseek64(fd, addr, SEEK_SET)) != -1)
    ret = write(fd, buf, size);

  close(fd);
  return ret;
}

u_int32_t sigScan(pid_t pid, u_int32_t start, u_int32_t end, std::string signature) {
    std::vector<char> bytePattern;
    std::vector<char> mask;
    parsePattern(signature, bytePattern, mask);

    // Attach to the target process
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("ptrace attach");
        return -1;
    }

    // Wait for the target process to stop
    waitpid(pid, NULL, 0);

    size_t patternLen = strlen(mask.data());

    // Try all possible starting addresses
    for (u_int32_t addr = start; addr < end; addr += 1) {
        bool match = true;

        // Read memory from the target process
        pokeData data;
        for (size_t i = 0; i < patternLen; i += sizeof(long)) {

          // Reads the word at addr + i
          data.val = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);

          // Compare the read data with the pattern
          for (size_t j = 0; j < sizeof(long); ++j) {
              if (mask[i + j] != '?' && bytePattern.data()[i + j] != data.chars[j]) {
                match = false;
                break;
              }
          }
          if (!match) {
              break;  // Break out if not a match
          }
        }

        if (match) {
            ptrace(PTRACE_DETACH, pid, NULL, NULL);
            return addr;
        }
    }

    // Detach from the target process
    ptrace(PTRACE_DETACH, pid, NULL, NULL);

    return -1; // Pattern not found
}

/**
 * Takes a IDA formatted pattern string, and fills a char vector and mask with it's byte representation
*/
void parsePattern(const std::string& patternStr, std::vector<char>& bytePattern, std::vector<char>& mask) {
  std::istringstream iss(patternStr);
  std::string token;
  while (std::getline(iss, token, ' ')) {
      if (token == "??") {
          bytePattern.push_back(0x0);
          mask.push_back('?');
      } else {
          std::stringstream ss(token);
          int byte;
          ss >> std::hex >> byte;
          bytePattern.push_back(static_cast<char>(byte));
          mask.push_back('x');
      }
  }
}
