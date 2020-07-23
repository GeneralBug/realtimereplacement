#if _WIN32
#   include <Windows.h>
#endif
#if __APPLE__
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glut.h>
#endif

# Linux (default)
TARGET = vector 
LDFLAGS = -lGL -lGLU -lglut -lm
CFLAGS = -Wall -std=c99

# Windows (cygwin)
ifeq "$(OS)" "Windows_NT"
TARGET = hello.exe
CFLAGS += -D_WIN32
endif

# OS X
ifeq "$(OSTYPE)" "darwin"
LDFLAGS = -framework Carbon -framework OpenGL -framework GLUT
CFLAGS += -D__APPLE__
endif


all:
	gcc -o $(TARGET) $(CFLAGS) $(LDFLAGS) vector.c
