// Compilation settings
// g++ -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

#include <iostream>
#include <cmath>

// Include SDL2 headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int y_direction = 100;

int x_direction = 100;


int main(int argc, char **argv) 
{

    bool IsFullscreen = false;

    SDL_Window *window;
    SDL_Renderer *renderer; 
    
    // initialize SDL window and renderer
    SDL_Init(SDL_INIT_EVERYTHING);


    SDL_DisplayMode DisplaySize;

    // Throws an error in the VS IDE, but somehow it still works...
    SDL_GetCurrentDisplayMode(0, &DisplaySize);

    // Use the width and height of the screen to make the window compatible with all screen resolutions.
    int WIDTH = DisplaySize.w/2;
    int HEIGHT = DisplaySize.h/1.4;

    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetWindowTitle(window, "Game window");
    SDL_RenderSetScale(renderer, 1, 1);

    if (NULL == window)
    {
        std::cout << "Could not create window" << SDL_GetError() << std::endl;
        return 1;
    }

    
    // Load textures
    SDL_Surface *image = IMG_Load("textures/PegBoard.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);

    // To make sure the pegboard does not get squished and retains the correct size with reuse the width of the screen.
    int SQUARE_WIDTH = WIDTH;

    SDL_Rect PegBoardRect = {0, -SQUARE_WIDTH/16, SQUARE_WIDTH, SQUARE_WIDTH-SQUARE_WIDTH/16};

    SDL_Event windowEvent;

    // Window loop
    while (true)
    {
        if (SDL_PollEvent(&windowEvent))
        {
            if (SDL_QUIT == windowEvent.type)
            {
                break;
            }

            // handle keyboard input
            if (SDL_KEYDOWN == windowEvent.type)
            {   
                // implementation of switching between fullscreen and windowed mode.
                if (windowEvent.key.keysym.sym == SDLK_F11)
                {
                    if (IsFullscreen == false)
                    {   
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        IsFullscreen = true;
                    }
                    else
                    {
                        SDL_SetWindowFullscreen(window, 0);
                        IsFullscreen = false;
                    }
                }                

                // y direction
                if (windowEvent.key.keysym.sym == SDLK_s)
                {
                    ++y_direction;
                }

                else if (windowEvent.key.keysym.sym == SDLK_w)
                {
                    --y_direction;
                }

                // x direction
                else if (windowEvent.key.keysym.sym == SDLK_d)
                {
                    ++x_direction;
                }

                else if (windowEvent.key.keysym.sym == SDLK_a)
                {
                    --x_direction;
                }
            }

        }

        // Actual drawing part
        SDL_SetRenderDrawColor(renderer, 30, 50, 100, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);

        SDL_RenderDrawLine(renderer, x_direction, y_direction, 300, 400);

        // Draw textures
        SDL_RenderCopy(renderer, texture, NULL, &PegBoardRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
