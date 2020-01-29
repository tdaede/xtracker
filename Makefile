
OUT_EXEC := xt

CC := human68k-gcc
OBJCOPY := human68k-objcopy
CFLAGS := -std=c99 -O2 -fshort-enums -fomit-frame-pointer -Wall -Werror -I$(HUMAN68K)/human68k/include -Isrc
LDFLAGS := -Wl,-q,-Map=$(OUT_EXEC).map -L$(HUMAN68K)/human68k/lib

SOURCES := $(wildcard src/*.c) $(wildcard src/*/*.c)
OBJECTS := $(SOURCES:.c=.o)


.PHONY: all clean

all: $(OUT_EXEC)

clean:
	$(RM) $(OBJECTS) $(OUT_EXEC).x $(OUT_EXEC).map
	rm -rf out/$(OUT_EXEC).x

$(OUT_EXEC): $(OBJECTS)
	$(CC) -o $(OUT_EXEC).bin $(LDFLAGS) $(CFLAGS) $(OBJECTS)
	mkdir -p out
	$(OBJCOPY) -v -O xfile $(OUT_EXEC).bin out/$(OUT_EXEC).x
	rm $(OUT_EXEC).bin

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
