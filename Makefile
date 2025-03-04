
EXEC = bin/huh
SRCDIR = ./src

## Compiler
CC = gcc
## Preproccessor flags
CPPFLAGS =
## Compiler flags
CFLAGS = 
## Linker flags
LDFLAGS = -I./src/glad -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm

DEBUGFLAGS = -g 

OBJS = $(SRCDIR)/main.o $(SRCDIR)/glad/glad.o $(SRCDIR)/debug.o

$(EXEC): $(OBJS) $(SHADERS)
		$(CC) -o $(EXEC) $(OBJS) $(LDFLAGS)

debug: CFLAGS += $(DEBUGFLAGS)
debug: clean $(EXEC)

run: $(EXEC)
	clear
	$(EXEC)


.PHONY: clean
clean:
		rm -rf $(SRCDIR)/*.o
		rm -rf $(SRCDIR)/glad/*.o
		rm -rf $(EXEC)

