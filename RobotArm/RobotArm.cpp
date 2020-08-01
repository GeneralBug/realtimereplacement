// Not actually a robot arm, just the first tutorial with axes

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#if _WIN32
#  include <Windows.h>
#endif
#if __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

#include <SDL.h>

typedef struct { float x, y; } vec2f;

SDL_Window* mainWindow = 0;

// Particles
const int n = 100;
vec2f r[n];
vec2f v[n];

void init()
{
    // Initialise particle positions and velocities here
}

void drawAxes(float length)
{
    glClear(GL_COLOR_BUFFER_BIT);
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    drawAxes(1.0);

    // Draw particles here. 

    SDL_GL_SwapWindow(mainWindow);

    // Always check for GL errors at least once per frame
    int err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("display: %s\n", gluErrorString(err));
    }

}

void update()
{
    // we'll switch between red and blue when the user presses a key:
    GLfloat colors[][3] = { { 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f } };
    static int back;
    glClearColor(colors[back][0], colors[back][1], colors[back][2], 1.0f);
        
    
    // Update positions here 
}

bool handleKeyDown(SDL_Keysym* key)
{
    // Handle key press events here
    switch (key->sym)
    {
    case SDLK_ESCAPE:
        return true;
        break;

    default:
        break;
    }

    return false;
}

bool handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_MOUSEMOTION:
            break;

        case SDL_MOUSEBUTTONDOWN:
            break;

        case SDL_KEYDOWN:
            return handleKeyDown(&event.key.keysym);
            break;

        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                if (event.window.windowID == SDL_GetWindowID(mainWindow))
                    SDL_SetWindowSize(mainWindow, event.window.data1, event.window.data2);
                break;
            case SDL_WINDOWEVENT_CLOSE:
                return false;
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    return false;
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return EXIT_SUCCESS;
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


    mainWindow = SDL_CreateWindow("Tutorial 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!mainWindow) {
        fprintf(stderr, "Failed to create a window: %s\n", SDL_GetError());
        return EXIT_SUCCESS;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GLContext mainGLContext = SDL_GL_CreateContext(mainWindow);
    if (mainGLContext == 0) {
        fprintf(stderr, "Unable to get OpenGL context: %s\n", SDL_GetError());
        return EXIT_SUCCESS;
    }

    if (SDL_GL_MakeCurrent(mainWindow, mainGLContext) != 0) {
        fprintf(stderr, "Unable to make OpenGL context current: %s\n", SDL_GetError());
        return EXIT_SUCCESS;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    int w, h;
    SDL_GetWindowSize(mainWindow, &w, &h);
    reshape(w, h);

    init();

    bool done = false;
    // Main event and display loop goes here
    while (!done)
    {
        done = handleEvents();
        update();
        drawAxes(1);
        SDL_GL_SwapWindow(mainWindow);
    }



    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    return EXIT_SUCCESS;
}

