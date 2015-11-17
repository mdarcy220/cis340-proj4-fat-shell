# Declaration of variables
CC = gcc
CC_FLAGS = -Wall -O2
CC_MAIN_METHOD_FLAG = 
LFLAGS = 

# File names
SRCDIR = .
OBJDIR = .
BINDIR = ..
EXECNAME = b.out
EXEC = $(BINDIR)/$(EXECNAME)
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

# Main target
.PHONY: all
all: $(BINDIR)/traverse $(BINDIR)/structure $(BINDIR)/showfile $(BINDIR)/showfat $(BINDIR)/showsector $(BINDIR)/help

.PHONY: debug
debug: CC_FLAGS += -DDEBUG -g -Og
debug: all

.PHONY: release
release: CC_FLAGS += -Werror
release: all


$(BINDIR)/help: CC_MAIN_METHOD_FLAG = -DHELP_MAIN
$(BINDIR)/help: $(OBJECTS)
	$(CC) -c $(CC_FLAGS) $(CC_MAIN_METHOD_FLAG) main.c -o main.o
	$(CC) $(OBJECTS) $(LFLAGS) -o $(BINDIR)/help
	
$(BINDIR)/showfat: CC_MAIN_METHOD_FLAG = -DSHOWFAT_MAIN
$(BINDIR)/showfat: $(OBJECTS)
	$(CC) -c $(CC_FLAGS) $(CC_MAIN_METHOD_FLAG) main.c -o main.o
	$(CC) $(OBJECTS) $(LFLAGS) -o $(BINDIR)/showfat
	
$(BINDIR)/showfile: CC_MAIN_METHOD_FLAG = -DSHOWFILE_MAIN
$(BINDIR)/showfile: $(OBJECTS)
	$(CC) -c $(CC_FLAGS) $(CC_MAIN_METHOD_FLAG) main.c -o main.o
	$(CC) $(OBJECTS) $(LFLAGS) -o $(BINDIR)/showfile
	
$(BINDIR)/showsector: CC_MAIN_METHOD_FLAG = -DSHOWSECTOR_MAIN
$(BINDIR)/showsector: $(OBJECTS)
	$(CC) -c $(CC_FLAGS) $(CC_MAIN_METHOD_FLAG) main.c -o main.o
	$(CC) $(OBJECTS) $(LFLAGS) -o $(BINDIR)/showsector
	
$(BINDIR)/structure: CC_MAIN_METHOD_FLAG = -DSTRUCTURE_MAIN
$(BINDIR)/structure: $(OBJECTS)
	$(CC) -c $(CC_FLAGS) $(CC_MAIN_METHOD_FLAG) main.c -o main.o
	$(CC) $(OBJECTS) $(LFLAGS) -o $(BINDIR)/structure
	
$(BINDIR)/traverse: CC_MAIN_METHOD_FLAG = -DTRAVERSE_MAIN
$(BINDIR)/traverse: $(OBJECTS)
	$(CC) -c $(CC_FLAGS) $(CC_MAIN_METHOD_FLAG) main.c -o main.o
	$(CC) $(OBJECTS) $(LFLAGS) -o $(BINDIR)/traverse


$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $(EXEC)

# To obtain object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
.PHONY: clean
clean:
	rm -f $(EXEC) $(OBJECTS) $(BINDIR)/traverse $(BINDIR)/structure $(BINDIR)/showfile $(BINDIR)/showfat $(BINDIR)/showsector $(BINDIR)/help
