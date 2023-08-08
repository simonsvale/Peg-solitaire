// Compilation settings
// g++ -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

#include <iostream>
#include <cmath>

// Include SDL2 headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


// TEST CODE REMOVE
int y_direction = 100;

int x_direction = 100;


// Function for deleting SDL textures and freeing surfaces
void Delete(SDL_Texture *TextureArr[], SDL_Surface *SurfaceArr[])
{
    // Loop through all textures and delete them from memory.
    for(int TextureNumber = 0; TextureNumber < (*(&TextureArr + 1) - TextureArr); TextureNumber++)
    {
        SDL_DestroyTexture(TextureArr[TextureNumber]);
    }

    // Loop through all surfaces and delete them from memory.
    for(int SurfaceNumber = 0; SurfaceNumber < (*(&SurfaceArr + 1) - SurfaceArr); SurfaceNumber++)
    {
        SDL_FreeSurface(SurfaceArr[SurfaceNumber]);
    }
}


SDL_Surface *LoadSurface(const char *ImagePath)
{   
    // Load image from given Path
    SDL_Surface *LoadedImage = IMG_Load(ImagePath);
    return LoadedImage;
}


SDL_Texture *LoadTexture(SDL_Surface *Surface, SDL_Renderer *renderer)
{
    SDL_Texture *SurfaceTexture = SDL_CreateTextureFromSurface(renderer, Surface);
    return SurfaceTexture;
}


int main(int argc, char **argv) 
{
    bool IsFullscreen = false;

    const char *ImagePathArray[] = {"textures/PegBoard.png", "textures/Peg.png", "textures/dummy.png"};
    int ImagePathArraySize = (*(&ImagePathArray + 1) - ImagePathArray);

    SDL_Surface *SurfaceArray[ImagePathArraySize];
    SDL_Texture *TextureArray[ImagePathArraySize];


    SDL_Window *window;
    SDL_Renderer *renderer; 
    
    // initialize SDL window and renderer
    SDL_Init(SDL_INIT_EVERYTHING);

    // Throws an error in the VS IDE, but somehow it still works...
    SDL_DisplayMode DisplaySize;
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
    
    // Create surfaces and textures and store them in ararys
    for(int ImagePathNumber = 0; ImagePathNumber < ImagePathArraySize; ImagePathNumber++)
    {
        SDL_Surface *Surface = LoadSurface(ImagePathArray[ImagePathNumber]);
        SDL_Texture *Texture = LoadTexture(Surface, renderer);

        // Store surfaces and textures in arrays to be able to delete them later
        SurfaceArray[ImagePathNumber] = Surface;
        TextureArray[ImagePathNumber] = Texture;
    }

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
        SDL_RenderCopy(renderer, TextureArray[0], NULL, &PegBoardRect);
        SDL_RenderCopy(renderer, TextureArray[1], NULL, &PegBoardRect);
        SDL_RenderCopy(renderer, TextureArray[2], NULL, &PegBoardRect);

        SDL_RenderPresent(renderer);
    }

    // Delete the Textures and Surfaces in their respective arrays
    Delete(TextureArray, SurfaceArray);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

