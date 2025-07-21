SDLFLAGS_CC:=$(shell pkg-config sdl3 --cflags)
SDLFLAGS_LD:=$(shell pkg-config sdl3 --libs)
CC:=$(shell which gcc)
LD:=$(shell which ld)

LIB_HEADERS:=$(shell find include -type f -name '*.h')
LIB_SRCFILES:=$(shell find src -type f -name '*.c')
LIB_OBJFILES:=$(patsubst %.c,%.o,$(LIB_SRCFILES))

LDFLAGS:=--static -L./lib
CFLAGS:=-g3 -gdwarf-2 -I./include/ -Wall -fstack-usage -Werror=analyzer-use-of-uninitialized-value -Werror=unused-result -Werror=multistatement-macros

LIBTARGET:=lib/libsdl3error.a

.PHONY: lib
.PHONY: clean
.PHONY: preprocessor
.PHONY: assembler
.PHONY: tests
.PHONY: coverage

all: $(LIBTARGET)

clean:
	rm -fr $(LIBTARGET) $(LIB_OBJFILES) 

src/%.o: src/%.c $(LIB_HEADERS)
	$(CC) -c -o $@ $(CFLAGS) -fprofile-arcs -ftest-coverage -fanalyzer $(SDLFLAGS_CC) $<

lib: $(LIBTARGET)

$(LIBTARGET): $(LIB_OBJFILES)
	mkdir -p lib
	ar rcs $(LIBTARGET) $(LIB_OBJFILES)
