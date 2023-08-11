// Compilation settings
// g++ -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -mconsole

#include <iostream>
#include <cmath>
#include <vector>

// Include SDL2 headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;

/*
    The ImagePathArray should not actually be Global, 
    but it is easier this way since the 
    ImagePathArraySize integer is needed in almost every function.
*/ 

const char *ImagePathArray[] = {"textures/dummy.png", "textures/PegBoard.png", "textures/Peg.png"};
const int ImagePathArraySize = (*(&ImagePathArray + 1) - ImagePathArray);

// TEST CODE REMOVE
int y_direction = 100;
int x_direction = 100;


// Function for deleting SDL textures and freeing surfaces
void Delete(SDL_Texture *TextureArr[], SDL_Surface *SurfaceArr[])
{      
    // Loop through all textures and surfaces and delete them from memory.
    for(int Number = 0; Number < ImagePathArraySize;)
    {
        SDL_DestroyTexture(TextureArr[Number]);
        SDL_FreeSurface(SurfaceArr[Number]);

        Number++;
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


void RenderEverything(SDL_Renderer *renderer, SDL_Texture *TextureArr[], vector<SDL_Rect> RectArr, vector<int> TextureAmountArr)
{   
    // Get size of RectArr, this is necessary since we need to render the peg image multiple times.
    int RectArrSize = RectArr.size();

    // Copy all loaded textures to the renderer (Overlays depends on the order of paths of the images)
    for(int TextureNumber = 0; TextureNumber < ImagePathArraySize;)
    {      
        if(TextureAmountArr[TextureNumber] > 1)
        {   
            for(int RenderAmount = 0; RenderAmount < TextureAmountArr[TextureNumber];)
            {
                int RectAmount = RenderAmount+TextureNumber;
                
                // Iterate through each rect in that "Block" of the same texture defined by the TextureAmountArray.
                SDL_Rect Rect[4] = {RectArr[RectAmount].x, RectArr[RectAmount].y, RectArr[RectAmount].w, RectArr[RectAmount].h};

                // Iterate through the Textures and their corrosponding SDL_Rects, defining their size on the screen.
                SDL_RenderCopy(renderer, TextureArr[TextureNumber], NULL, Rect);

                RenderAmount++;
            }
        }
        else
        {   
            // Else just render the one texture
            SDL_Rect Rect[4] = {RectArr[TextureNumber].x, RectArr[TextureNumber].y, RectArr[TextureNumber].w, RectArr[TextureNumber].h};
            SDL_RenderCopy(renderer, TextureArr[TextureNumber], NULL, Rect);
        }

        TextureNumber++;
    }
}


int main(int argc, char **argv) 
{
    bool IsFullscreen = false;

    // For storing the actual images, that are used as textures.
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
    for(int ImagePathNumber = 0; ImagePathNumber <= ImagePathArraySize-1;)
    {
        SDL_Surface *Surface = LoadSurface(ImagePathArray[ImagePathNumber]);
        SDL_Texture *Texture = LoadTexture(Surface, renderer);

        // Store surfaces and textures in arrays to be able to delete them later
        SurfaceArray[ImagePathNumber] = Surface;
        TextureArray[ImagePathNumber] = Texture;

        ImagePathNumber++;
    }

    // Needs pointers because of the use of arrays in arrays 
    SDL_Rect PegBoardRect = {0, -80, WIDTH, WIDTH}; 

    /* 
    All basic pegs need to be made in a for loop !!!, likewise the animation when they jump, 
    should be a precalcalculated parabola, with x, y being the two first integers in the SDL_Rect.
    Because of this jump, the pegs "closest" to the player should be rendered ontop of the ones further away.
    */
    SDL_Rect PegRect = {300, 79, int(WIDTH/9.3), int(WIDTH/9.3)};

    SDL_Rect DummyRect = {90, 90, 90, 90};


    vector<int> TextureAmountArray = {1, 1, 2};

    SDL_Event windowEvent;

    // DEBUG TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    int AdditionNumber = 0;

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
        SDL_Rect PegRect2 = {400+AdditionNumber, 79, int(WIDTH/9.3), int(WIDTH/9.3)};

        // In order for animations to work, update the Rect array.
        vector<SDL_Rect> RectArray = {DummyRect, PegBoardRect, PegRect, PegRect2};

        // Actual drawing part
        SDL_SetRenderDrawColor(renderer, 30, 50, 100, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);

        SDL_RenderDrawLine(renderer, x_direction, y_direction, 300, 400);

        // RectArray is a vector class instead of a double pointer.
        RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray);

        // Renderer the loaded textures
        SDL_RenderPresent(renderer);

        // DEBUG TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        AdditionNumber++;
    }

    // Delete the Textures and Surfaces in their respective arrays
    Delete(TextureArray, SurfaceArray);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

