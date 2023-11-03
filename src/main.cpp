#include <cstdio>
#include <cassert>

#include <SDL2/SDL.h>
#include <Volk/volk.h>

#include "rendering/context.h"

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Voxel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_VULKAN);
    if (!window) {
        printf("Failed to create SDL window: %s", SDL_GetError());
        return 1;
    }

    Result r = InitializeRenderContext(window);
    if (r != Success) {
        printf("Failed to initialize rendering: %d\n", r);
        return 1;
    }

    bool running = true;
    while (running) {
        SDL_Event e = {};
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        RenderFrame();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}