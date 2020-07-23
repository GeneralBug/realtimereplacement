CC = gcc
CFLAGS = -lglut -lGLU -lGL
DEPS = utils.h shapes.h s3718372.h
OBJ = utils.o shapes.o s3718372.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

frogger.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
