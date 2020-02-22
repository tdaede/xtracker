
OUT_EXEC := xt

CC := m68k-linux-gnu-gcc
OBJCOPY := human68k-objcopy
CFLAGS := -std=c99 -O2 -fshort-enums -fomit-frame-pointer -Wall -I$(HUMAN68K)/human68k/include -Isrc -nostdlib -flto
LDFLAGS :=  $(HUMAN68K)/human68k/lib/crt0.o -Wl,-q,-Map=$(OUT_EXEC).map,-T/home/thomas/sandbox/test-x68k/human68kxfile.x,-L$(HUMAN68K)/human68k/lib,-lc,-liocs,-ldos,-lgcc

OBJECTS := $(SOURCES:.c=.o)

SOURCES := $(shell find ./$(SRCDIR)/ -type f -name '*.c')
RESOURCES := $(shell find ./$(RESDIR)/ -type f -name '*.rc')
OBJECTS := $(SOURCES:.c=.o)

.PHONY: all clean resources

all: $(OUT_EXEC)

clean:
	$(RM) $(OBJECTS) $(OUT_EXEC).x $(OUT_EXEC).map
	rm -rf ./out/

resources:
	@mkdir -p out
	@cp -r res/* out/

$(OUT_EXEC): $(OBJECTS) resources
	@bash -c 'printf "\t\e[94m[ LNK ]\e[0m $(OBJECTS)\n"'
	@$(CC) -o $(OUT_EXEC).bin $(CFLAGS) $(OBJECTS) $(LDFLAGS)
	@mkdir -p out
	@$(OBJCOPY) -v -O xfile $(OUT_EXEC).bin out/$(OUT_EXEC).x > /dev/null
	@rm $(OUT_EXEC).bin
	@bash -c 'printf "\e[92m\n\tBuild Complete. \e[0m\n\n"'

%.o: %.c
	@bash -c 'printf "\t\e[96m[  C  ]\e[0m $<\n"'
	@$(CC) -c $(CFLAGS) $< -o $@
