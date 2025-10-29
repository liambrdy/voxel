#include <cstdio>
#include <cassert>

#include <SDL2/SDL.h>
#include <Volk/volk.h>
#include <glm/glm.hpp>

#include "rendering/context.h"

const int WIDTH = 64;
const int HEIGHT = 64;
const int DEPTH = 64;

int positionToIndex(const glm::uvec3 &pos) {
    return pos.x + pos.y * WIDTH + pos.z * WIDTH * HEIGHT;
}

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

    glm::ivec3 center(WIDTH / 2, HEIGHT / 2, DEPTH / 2);

    std::vector<int> voxels(WIDTH * HEIGHT * DEPTH);
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int z = 0; z < DEPTH; z++) {
                int index = positionToIndex(glm::vec3(x, y, z));

                int newx = (x - center.x)*(x - center.x);
                int newy = (y - center.y)*(y - center.y);
                int newz = (z - center.z)*(z - center.z);
                if (newx + newy + newz < WIDTH*WIDTH/16) {
                    voxels[index] = 1;
                } else {
                    voxels[index] = 0;
                }
            }
        }
    }

    UploadVoxelData(voxels);

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