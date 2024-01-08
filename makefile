src := $(wildcard src/*.c)
output := sfvm
cflags := -O2 -Wall -Wextra -Werror

all: build run
build: 
	gcc $(src) -o $(output) $(cflags)
run: 
	./$(output)
