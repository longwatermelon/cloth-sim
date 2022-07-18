CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror
INC=-Ideps/include
LIBS=-Ldeps/lib -lglfw -lcglm -lm -lglad -lstb_image -lassimp

SRC=$(wildcard src/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

all:
	mkdir -p obj/src
	$(MAKE) target

target: $(OBJS)
	$(CC) $(CFLAGS) $(INC) $^ $(LIBS)

obj/src/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ $(LIBS)

clean:
	-rm -rf obj/ a.out

