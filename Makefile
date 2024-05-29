CC := g++

TARGET  := L4D2Cheat
VERSION := $(shell head -n 1 VERSION)

DIR     ?= /usr/local/sbin

ifeq ($(RELEASE),1)
  CPPFLAGS := -DNDEBUG
endif

ifeq ($(DEBUG),1)
  CFLAGS := -O0 -g -fsanitize=undefined,address
else
  CFLAGS := -O3
endif

LDFLAGS  := -pthread
CPPFLAGS += -DVERSION=\"$(VERSION)\"
CFLAGS   += -m32 \
            -pedantic \
            -Wall \
            -Wextra \
            -Wno-deprecated-declarations \
            -Iinclude \
            $(CPPFLAGS)

SRC := src/commands.cpp \
       src/game.cpp \
       src/handler.cpp \
       src/keyboard.cpp \
       src/main.cpp \
       src/mem.cpp \
       src/utils.cpp

OBJ := $(SRC:.cpp=.o)


all: $(TARGET)

.cpp.o:
	$(CC) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $+ -lX11 -lXtst

install: all
	mkdir -p $(DIR)
	cp -f $(TARGET) $(DIR)/$(TARGET)
	chmod 755 $(DIR)/$(TARGET)

uninstall:
	@rm -vf $(DIR)/$(TARGET)

clean:
	@rm -vf $(OBJ) $(TARGET)
