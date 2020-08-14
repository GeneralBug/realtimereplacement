#include <iostream>
#include "SDL.h"

//https://matthewstyles.com/writing-an-sdl2-game-loop/ example epilepsy generator, changed to not kill people

int main(int argc, char* argv[])
{
    printf("hello world\n");

    SDL_Window* window = SDL_CreateWindow("Demo Game",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        SDL_Log("Could not create a window: %s", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Could not create a renderer: %s", SDL_GetError());
        return -1;
    }

    bool done = false;
    bool redFlag = true;

    Uint8 red = 255;
    Uint8 green = 0;

    while (!done)
    {
        //EVENT HANDLING
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                done = true;
                break;
            case SDL_KEYDOWN:
                printf("Key press detected\n");
                redFlag = !redFlag;
                break;
            }
        }

        //UPDATE
        if (redFlag)
        {
            red = 255;
            green = 0;
        }
        else
        {
            red = 0;
            green = 255;
        }

        //RENDER
        SDL_SetRenderDrawColor(renderer, red, green, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    // Tidy up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}