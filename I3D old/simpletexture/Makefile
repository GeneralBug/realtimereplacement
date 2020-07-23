CC = gcc

# Linux (default)
TARGET = textureViewer
CFLAGS = -Wall -I.
LDFLAGS = -lGL -lGLU -lglut -lm ./libSOIL.a

# Windows (cygwin)
ifeq "$(OS)" "Windows_NT"
	TARGET = textureViewer.exe
	CFLAGS += -D_WIN32
endif

# OS X
ifeq "$(OSTYPE)" "darwin"
	LDFLAGS = -framework Carbon -framework QuickTime -framework OpenGL -framework GLUT
	CFLAGS = -D__APPLE__ -Wall
endif


SRC = textureViewer.c
OBJ = textureViewer.o


all: $(TARGET)

debug: CFLAGS += -g
debug: all

$(TARGET): $(OBJ)
	@echo linking $(TARGET)
	@$(CC) -o $(TARGET) $(OBJ) $(CFLAGS) $(LDFLAGS)

%.o: %.c
	@echo compiling $@ $(CFLAGS)
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	@echo cleaning $(OBJ)
	@rm $(OBJ)
