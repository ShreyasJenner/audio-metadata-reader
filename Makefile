# Compiler and flags
CC=gcc
CFLAGS=-Wall -Iinclude -lFLAC

# Directories
SRCDIR=src
OBJDIR=obj
LIBDIR=lib
BINDIR=bin

# Files
SRC=$(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJ=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
LIB=$(LIBDIR)/metadata-reader.a
BIN=$(BINDIR)/metadata-reader

# compile objects 
all: $(BIN)

$(BIN): $(LIB) | $(BINDIR)
	$(CC) -o $(BIN) $(OBJ) $(CFLAGS)

$(BINDIR):
	@mkdir -p bin

$(LIB): $(OBJ) | $(LIBDIR)
	ar -rcs $@ $(OBJ) 

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)
	rm -f $(LIB)


info:
	$(info SRC=$(SRC))

.PHONY: all clean
