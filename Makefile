# Latest version available at https://gist.github.com/pouya-abbassi/2be0785705ac091d1d194737c80cd063

# The compiler: gcc for C program.
CC = clang

# Optimize as much as possible.
SPEED = -s -O2
SIZE = -Oz
# GNU dialect of ISO C17.
CFLAGS += -std=c17
# Turn on most, but not all compiler warnings.
CFLAGS += -Wall
# Turn on some extra warnings.
CFLAGS += -Wextra
# Turn warnings into errors.
#CFLAGS += -Werror
# Warn if floating-point values are used in quality comparisons.
CFLAGS += -Wfloat-equal
# Warn whenever a local variable or type declaration shadows something.
CFLAGS += -Wshadow
# Warn if a function is declared or defined without specifying the argument types.
CFLAGS += -Wstrict-prototypes
# Warn if a global function is defined without a previous prototype declaration.
CFLAGS += -Wmissing-prototypes
# Error if a function is undeclared.
CFLAGS += -Werror-implicit-function-declaration
# Warn whenever a `switch` statement does not have a `default` case.
#CFLAGS += -Wswitch-default
# Warn if a variable is used without first being initialized.
CFLAGS += -Wuninitialized
# Warn about variables that are initialized with themselves.
CFLAGS += -Winit-self
# Warn about duplicated conditions in a if-else-if chain.
#CFLAGS += -Wduplicated-cond
# Warn when an if-else-if has identical branches.
#CFLAGS += -Wduplicated-branches
# Warn about uses of logical operators in where a bit-wise operator is expected.
#CFLAGS += -Wlogical-op
# Warn if the compiler detects path that might dereference a Null pointer.
CFLAGS += -Wnull-dereference
# Issue all the warnings demanded by strict ISO C.
CFLAGS += -pedantic
# Generates traps for signed overflow on addition; Slightly slower runtime!
CFLAGS += -ftrapv
# Specify the name of the target architecture.
# CFLAGS += -march=native


SDL = `sdl2-config --libs --cflags`

TARGET = main

build:
	$(CC) $(CFLAGS) $(SDL) $(TARGET).c -o bin/$(TARGET) && ./bin/main

wasm:
	emcc $(TARGET).c --shell-file=shell_minimal.html -s USE_SDL=2 -s ASYNCIFY=1 -o public_html/index.html

release:
	$(CC) $(SPEED) $(CFLAGS) $(SDL) $(TARGET).c -o bin/$(TARGET)
	emcc $(SIZE) $(TARGET).c  -s USE_SDL=2 -s ASYNCIFY=1 -o public_html/index.html


temps:
	$(CC) $(SDL) -O3 -std=gnu17 -save-temps -fverbose-asm -masm=intel bin/$(TARGET).c

run:
	./bin/$(TARGET)

eglot:
	bear --output compile_command.json -- make

clean:
	rm bin/* public_html/*
