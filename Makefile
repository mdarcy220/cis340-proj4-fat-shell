# Declaration of variables
CC = gcc
CC_FLAGS = -Wall
LFLAGS = 

# File names
SRCDIR = .
OBJDIR = .
BINDIR = .
EXECNAME = a.out
EXEC = $(BINDIR)/$(EXECNAME)
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

# Main target
all: $(EXEC)
	cd builtin_commands && $(MAKE) all

debug: CC_FLAGS += -DDEBUG -g
debug: $(EXEC)
	cd builtin_commands && $(MAKE) debug

release: CC_FLAGS += -Werror
release: $(EXEC)
	cd builtin_commands && $(MAKE) release

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $(EXEC)

# To obtain object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
	cd builtin_commands && $(MAKE) clean
