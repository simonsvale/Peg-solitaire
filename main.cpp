// Compilation settings
// g++ -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -mconsole

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include<windows.h> 

// Include SDL2 headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


// Include own header file(s)
#include "animations.h"

using namespace std;


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


// Loads surfaces from an image path
SDL_Surface *LoadSurface(const char *ImagePath)
{   
    // Load image from given Path
    SDL_Surface *LoadedImage = IMG_Load(ImagePath);
    return LoadedImage;
}


// Loads texture from a surface
SDL_Texture *LoadTexture(SDL_Surface *Surface, SDL_Renderer *renderer)
{
    SDL_Texture *SurfaceTexture = SDL_CreateTextureFromSurface(renderer, Surface);
    return SurfaceTexture;
}

// Renders everything
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
    // For determining if the window is minimized or not.
    bool IsWindowActive = true;

    // Create the game tick variable.
    int GameTick = 0;

    // To make sure peg animations don't happen at the same time.
    bool IsAnimationActive = false;
    
    // To distinguish between a selected peg with outline and one that isn't
    bool IsPegSelected = false;
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
    vector<int> ScreenResolution = {WIDTH, HEIGHT};


    // Create the SDL window and set its title
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetWindowTitle(window, "Peg Solitaire");
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

    // Vector for storing Rects.
    vector<SDL_Rect> RectArray;

    // peg width = 43

    // Push Rects to the RectArray
    RectArray.push_back({90, 90, 90, 90}); // Dummy texture rect
    RectArray.push_back({0, -80, WIDTH, WIDTH}); // board texture rect

    // A vector for holding Integers determining the spacing between pegs.
    vector<vector<int> > BoardConfigurationArray = {{129}, {60, 63}};

    vector<int> PegWidth = {43, 42, 43, 42, 43, 42, 43};


    // Create all 32 pegs and push them to the RectArray.
    for(int PegNumber = 0; PegNumber < 32;)
    {
        RectArray.push_back({1,2,3,4});

        PegNumber++;
    }

    RectArray.push_back({330, 93, int(WIDTH/22.1), int(WIDTH/22.1*2.07)});
    RectArray.push_back({459, 93, int(WIDTH/22.1), int(WIDTH/22.1*2.07)});
    RectArray.push_back({588, 183, int(WIDTH/22.1), int(WIDTH/22.1*2.07)});
    RectArray.push_back({172, 258, int(WIDTH/22.1), int(WIDTH/22.1*2.07)});


    vector<int> TextureAmountArray = {1, 1, int(RectArray.size()-2)};


    // DEBUG!!!! Should take any Peg's current position.
    // And needs two, since there is either 43 (129) or 42 (128) pixels between each peg
    vector<vector<int> > Hello = PegJumpAnimation(330, 93, 129, 0);

    // Setup SDL variables
    SDL_Event windowEvent;
    SDL_Point MousePos;


    // Set background color
    SDL_SetRenderDrawColor(renderer, 30, 50, 100, 255);
    SDL_RenderClear(renderer);
    

    // Initial renderer
    RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray);
    SDL_RenderPresent(renderer);

    // Game loop
    while (true)
    {
        // Get mouse postion and window flag
        SDL_GetMouseState(&MousePos.x, &MousePos.y);
        int WindowFlag = SDL_GetWindowFlags(window);

        /* If the window gains focus render everything again.
        Works flawlessly except for the edgecase where the taskbar makes 
        SDL_GetWindowFlags() think the window is hidden since the taskbar is alway ontop.*/
        if((WindowFlag == 8260) || (IsWindowActive == false))
        {
            IsWindowActive = false;

            if(WindowFlag == 8708)
            {
                IsWindowActive = true;

                SDL_SetRenderDrawColor(renderer, 30, 50, 100, 255);
                SDL_RenderClear(renderer);
                RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray);
                SDL_RenderPresent(renderer);
            }
        }

        if (SDL_PollEvent(&windowEvent))
        {
            if (SDL_QUIT == windowEvent.type)
            {
                break;
            }

            if (SDL_MOUSEBUTTONDOWN == windowEvent.type)
			{  

                // DEBUG !!!!!!!!
                cout << MousePos.x << ", " << MousePos.y << endl;

                // Check if mouse's position is ontop of the texture.
                if(SDL_PointInRect(&MousePos, &RectArray[2]))
                {
                    cout << "Yep" << endl;

                    IsAnimationActive = true;

                    /*
                        1. Switch all Pegs to texture with no outline.
                        2. Switch selected peg to texture with outline.
                        3. Change the bool IsPegSelected to True.
                    */
                }
    
            }

            if(IsPegSelected == true)
            {
                /* 4. Run function: GetPossibleMoves(), returns an array with possible moves as Rects.
                   5. If statement to check if any possible moves have been selected using SDL_PointInRect(&MousePos, &PegRect),
                      but comparing it to the Rects from the GetPossibleMoves() function.
                   6. Run the AnimationCall Function, for the selected Peg. Update Peg to its new position.
                */
                
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

        if(IsAnimationActive == true)
        {
            if(GameTick < Hello.size())
            {
                // Update RectArray to do animation
                RectArray[2] = {Hello[GameTick][0], Hello[GameTick][1], int(WIDTH/22.1), int(WIDTH/22.1*2.07)};

                // Render the animation
                RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray);
                SDL_RenderPresent(renderer);
            }
            else
            {
                IsAnimationActive = false;
            }

            GameTick++;
        }
        
        // Set Gametick to zero again so the next event can happen
        if(IsAnimationActive == false)
        {
            GameTick = 0;
        }
    }

    // Delete the Textures and Surfaces in their respective arrays
    Delete(TextureArray, SurfaceArray);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

