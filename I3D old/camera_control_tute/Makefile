CC = gcc
CFLAGS = -lglut -lGLU -lGL
DEPS = shapes.h utils.h sinwave.h
OBJ = shapes.o utils.o sinwave.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

camera.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
