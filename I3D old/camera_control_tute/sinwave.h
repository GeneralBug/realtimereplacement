#include "shapes.h"

void keyboardCB (unsigned char, int, int);
void display();

typedef struct { bool debug; float teapot_scale; bool lit, wire; } global;