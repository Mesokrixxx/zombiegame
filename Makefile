NAME = zombiegame

CC = clang
CFLAGS = -Wall -Wextra -I. -g3 -std=c23 -Wno-missing-field-initializers
LDFLAGS = -g3 -lm

SRC = src/main.c src/sprites.c src/fonts.c src/ecs.c
SRC += engine/engine.c engine/window.c engine/inputs.c engine/time.c
SRC += memory/allocator.c memory/mallocator.c
SRC += math/vector2.c math/vector3.c math/vector4.c math/mat4.c
SRC += gl/shader.c gl/buffer.c gl/texture.c gl/types.c gl/draw.c
SRC += container/dynlist.c container/string.c container/map.c container/bitset.c container/sparseset.c
SRC += util/assert.c util/log.c util/error.c util/file.c util/stringview.c util/hash.c

LIBS = sdl3 glew

OBJ = $(SRC:.c=.o)
MAKEFLAGS += -j --no-print-directory
CFLAGS += $(shell pkg-config --cflags $(LIBS))
LDFLAGS += $(shell pkg-config --libs $(LIBS))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(NAME) $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean
	$(MAKE) all

.PHONY = all clean fclean re
