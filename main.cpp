// Compilation settings
// g++ -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -mconsole

// Include necessary built-in header files
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <windows.h> 
#include <chrono>

// Include non built-in header files
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Include own header file(s)
#include "animations.h"

using namespace std;
using namespace chrono;

// Const char for holding the position of the textures.
const char *ImagePathArray[] = {"textures/dummy.png", "textures/PegBoard.png", "textures/Peg.png", "textures/HoleSelect.png", "textures/PegOutline.png"};
const int ImagePathArraySize = (*(&ImagePathArray + 1) - ImagePathArray);

// Struct for clicked peg return
struct ClickedPeg
{
    int RectNumber;
    bool IsSelected;
};

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


// Function for loading a surface from an image path
SDL_Surface *LoadSurface(const char *ImagePath)
{   
    // Load image from given Path
    SDL_Surface *LoadedImage = IMG_Load(ImagePath);
    return LoadedImage;
}


// Function for loading a texture from a surface
SDL_Texture *LoadTexture(SDL_Surface *Surface, SDL_Renderer *renderer)
{
    SDL_Texture *SurfaceTexture = SDL_CreateTextureFromSurface(renderer, Surface);
    return SurfaceTexture;
}


// A function for rendering everything each time its called, can exclude textures
void RenderEverything(SDL_Renderer *renderer, SDL_Texture *TextureArr[], vector<SDL_Rect> RectArr, vector<int> TextureAmountArr, ClickedPeg Peg, int ArraySum)
{   
    // Copy all loaded textures to the renderer (Overlays depends on the order of paths of the images), 
    // -1 is because this is the amount of images from the ImagePathArray we do not won't to render.
    int FromPosArraySum = 0;
    int FromPos;

    for(int TextureNumber = 0; TextureNumber < ImagePathArraySize-1;)
    {      
        if(TextureAmountArr[TextureNumber] > 1)
        {   
            FromPos = TextureNumber;
            FromPosArraySum = 0;

            // Calculate the FromPosArraySum, used to determine the position of the correct Rect in the RectArray
            for(; FromPos < TextureAmountArr.size();)
            {   
                FromPosArraySum += TextureAmountArr[FromPos];
                FromPos++;
            }

            for(int RenderNumber = 0; RenderNumber < TextureAmountArr[TextureNumber];)
            {   
                int RectPos = ArraySum - FromPosArraySum + RenderNumber;

                // Iterate through each rect in that "Block" of the same texture defined by the TextureAmountArray.
                SDL_Rect Rect[4] = {RectArr[RectPos].x, RectArr[RectPos].y, RectArr[RectPos].w, RectArr[RectPos].h};

                // -2 because this lines up with the actual render number for the outlined peg.
                if((Peg.IsSelected == true) && (RenderNumber == Peg.RectNumber-2) && (TextureNumber == 2))
                {
                    // Render the selected peg texture / outlined peg.
                    SDL_RenderCopy(renderer, TextureArr[4], NULL, Rect);
                }
                else
                {   
                    // Render all other pegs.
                    // Iterate through the Textures and their corrosponding SDL_Rects, defining their size on the screen.
                    SDL_RenderCopy(renderer, TextureArr[TextureNumber], NULL, Rect);
                }

                RenderNumber++;
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

// Function for detecting when a sprite is clicked.
ClickedPeg SpriteClickDetection(SDL_Point MousePos, vector<SDL_Rect> RectArray)
{
    // Struct for holding IsSelected (bool) and RectNumber (int)
    ClickedPeg returnVal;

    for(int RectNumber = 2; RectNumber < RectArray.size()-3;)
    {
        // Take mouse pos and see if it is inside the boundary of any peg sprite.
        if(SDL_PointInRect(&MousePos, &RectArray[RectNumber]) == true)
        {
            // If is is set the structs members to the correct information.
            returnVal.IsSelected = true;
            returnVal.RectNumber = RectNumber;

            return returnVal;
        }

        RectNumber++;
    }

    // If the mouse position is not set them to the following:
    returnVal.IsSelected = false;

    // -1 because it will never be used otherwise, since the RectNumber's range can only be from 0 to x.
    returnVal.RectNumber = -1;

    return returnVal;
}

// Function for getting the possible moves for the selected peg.
vector<bool> GetPossibleMoves(int PegPosition, vector<int> CurrentBoardLayout)
{
    vector<bool> Moves = {false, false, false, false};

    int Pos = PegPosition;

    vector<int> CheckPegLocations = {PegPosition-1, PegPosition+1, PegPosition+7, PegPosition-7};
    vector<int> CheckEmptyLocations = {PegPosition-2, PegPosition+2, PegPosition+14, PegPosition-14};

    for(int Count = 0; Count < 4;)
    {
        // Check if there is a peg on each side of the selected peg
        if(CurrentBoardLayout[CheckPegLocations[Count]] == 1)
        {
            // If there is check if there is an empty space beside that peg.
            if(CurrentBoardLayout[CheckEmptyLocations[Count]] == 0)
            {
                Moves[Count] = true;
            }
            else
            {
                Moves[Count] = false;
            }
        }
        else
        {
            Moves[Count] = false;
        }

        Count++;
    }

    return Moves;
}


// Main function
int main(int argc, char **argv) 
{   
    // For determining if the window is minimized or not.
    bool IsWindowActive = true;

    // Create the game tick variable.
    int GameTick = 0;
    int OutlineTick = 0;

    // To make sure peg animations don't happen at the same time.
    bool IsAnimationActive = false;

    // Bool for when a position for the clicked peg is selected.
    bool IsJumpPositionSelected = false;

    // Bool for determining if the selected peg outline have been rendered.
    bool IsOutlineRendered = false;
    vector<int> PreviousRectNumber;

    // Bool for determining if the window is in fullscreen mode
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
        cout << "Could not create window" << SDL_GetError() << endl;
        return 1;
    }
    
    // Create surfaces and textures and store them in ararys
    for(int ImagePathNumber = 0; ImagePathNumber <= ImagePathArraySize-1;)
    {
        // Load the surfaces and textures.
        SDL_Surface *Surface = LoadSurface(ImagePathArray[ImagePathNumber]);
        SDL_Texture *Texture = LoadTexture(Surface, renderer);

        // Store surfaces and textures in arrays to be able to delete them later
        SurfaceArray[ImagePathNumber] = Surface;
        TextureArray[ImagePathNumber] = Texture;

        ImagePathNumber++;
    }

    // All 33 positions, 1 = populated, 0 = empty.
    vector<int> BoardLayout = {-1, -1, 1, 1, 1, -1, -1,
                               -1, -1, 1, 1, 1, -1, -1,
                                 1, 1, 1, 1, 1, 1, 1, 
                                 1, 1, 1, 0, 1, 1, 1, 
                                 1, 1, 1, 1, 1, 1, 1, 
                               -1, -1, 1, 1, 1, -1, -1,
                               -1, -1, 1, 1, 1, -1, -1};

    // Struct for storing possible moves when selecting a peg. 
    //                               North, South, East,  West
    vector<bool> SelectedPegMoves = {false, false, false, false};

    vector<bool> test = GetPossibleMoves(17, BoardLayout);

    for(int count = 0; count < 4;)
    {
        cout << test[count] << endl;
        count++;
    }

    // Vector for storing Rects.
    vector<SDL_Rect> RectArray;

    // Push Rects to the RectArray
    RectArray.push_back({90, 90, 90, 90}); // Dummy texture rect
    RectArray.push_back({0, -80, WIDTH, WIDTH}); // board texture rect

    // Setup positions
    vector<int> PegSetupX = {73, 202, 330, 458, 587, 715, 843};
    vector<int> PegSetupY = {4, 94, 184, 278, 371, 461, 551};

    // Create the board of 7x7 pegs.
    for(int PegPosX = 0; PegPosX < 7;)
    {
        for(int PegPosY = 0; PegPosY < 7;)
        {
            RectArray.push_back({PegSetupX[PegPosX], PegSetupY[PegPosY], int(WIDTH/22.1), int(WIDTH/22.1*2.07)});

            PegPosY++;
        }

        PegPosX++;
    }

    // Remove pins outside the board on the left
    RectArray.erase(RectArray.begin()+2, RectArray.begin()+4);
    RectArray.erase(RectArray.begin()+5, RectArray.begin()+9);
    RectArray.erase(RectArray.begin()+8, RectArray.begin()+10);

    // Remove middle pin
    RectArray.erase(RectArray.begin()+18);

    // Remove pins outside the board on the right
    RectArray.erase(RectArray.end()-2, RectArray.end());
    RectArray.erase(RectArray.end()-7, RectArray.end()-3);
    RectArray.erase(RectArray.end()-8, RectArray.end()-6);

    // Create the 4 possible holes to select
    for(int XVal = 0; XVal < 4;)
    {
        RectArray.push_back({PegSetupX[XVal]+128, PegSetupY[XVal]-38+100, int(WIDTH/22.1), int(WIDTH/22.1/1.25)});
        XVal++;
    }

    // An array for determining how many times a texture should be used to make a sprite.
    vector<int> TextureAmountArray = {1, 1, 32, 4};

    // Calculate the total amount of all textures in the TextureAmountArray.
    int ArraySum = 0;

    for(int ArrayIndex = 0; ArrayIndex < TextureAmountArray.size();)
    {
        ArraySum += TextureAmountArray[ArrayIndex];
        ArrayIndex++;
    }

    // A vector for holding the frames of an animation
    vector<vector<int> > PegJumpAnimationFrames;

    // Setup SDL variables
    SDL_Event windowEvent;
    SDL_Point MousePos;

    // Setup Struct
    ClickedPeg SpriteInfo; SpriteInfo.IsSelected = false; SpriteInfo.RectNumber = -1;

    // Set background color
    SDL_SetRenderDrawColor(renderer, 30, 50, 100, 255);
    SDL_RenderClear(renderer);
    
    // Initial renderer
    RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
    SDL_RenderPresent(renderer);

    // Main loop
    while (true)
    {
        // Get mouse postion and window flag
        SDL_GetMouseState(&MousePos.x, &MousePos.y);
        int WindowFlag = SDL_GetWindowFlags(window);

        /* If the window gains focus, render everything again.
        Works flawlessly except for the edgecase where the taskbar makes 
        SDL_GetWindowFlags() think the window is hidden, since the taskbar is alway ontop.*/
        if((WindowFlag == 8260) || (IsWindowActive == false))
        {
            IsWindowActive = false;

            if(WindowFlag == 8708)
            {
                IsWindowActive = true;

                SDL_SetRenderDrawColor(renderer, 30, 50, 100, 255);
                SDL_RenderClear(renderer);

                RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
                SDL_RenderPresent(renderer);
            }
        }

        if (SDL_PollEvent(&windowEvent))
        {
            if (SDL_QUIT == windowEvent.type)
            {
                break;
            }

            if ((SDL_MOUSEBUTTONDOWN == windowEvent.type) && (IsAnimationActive == false))
			{  
                // Detect if any sprite was clicked/selected, and return a struct with that information.
                SpriteInfo = SpriteClickDetection(MousePos, RectArray);
            }

            // Check if a sprite was selected.
            if(SpriteInfo.IsSelected == true)
            {   
                // To not spam the rendering function, check if the previous mouse click was deselecting the (Not same) peg or the outline tick is 0, then render
                if((OutlineTick == 0) || (SpriteInfo.RectNumber != PreviousRectNumber.back()))
                {   
                    IsOutlineRendered = false;
                    OutlineTick++;
                }

                // Check if a jump position have been set.
                if(IsJumpPositionSelected == true)
                {
                    // Set animation bool to true.
                    IsAnimationActive = true;
                }

                PreviousRectNumber.clear();
                PreviousRectNumber.push_back(SpriteInfo.RectNumber);
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

                // DEBUG for JumpPosSelection
                if (windowEvent.key.keysym.sym == SDLK_s)
                {
                    IsJumpPositionSelected = true;
                }
            }
        }

        // Check if a peg have been selected
        if(SpriteInfo.IsSelected == true)
        {   
            if((IsJumpPositionSelected == false) && (IsOutlineRendered == false))
            {
                // Render selected peg with outline
                RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
                SDL_RenderPresent(renderer);

                IsOutlineRendered = true;
            }

            if(IsJumpPositionSelected == false)
            {
                // Get the possible moves.
                SelectedPegMoves = GetPossibleMoves(SpriteInfo.RectNumber, BoardLayout);

                // Update board layout.
            }
            
            // If a JumpPos have been selected then the animation
            if((IsAnimationActive == true) && (IsJumpPositionSelected == true))
            {   
                // Start timing, using chrono (DEBUG)
                time_point<steady_clock> start = steady_clock::now();

                if(PegJumpAnimationFrames.size() == 0)
                {
                    // The values should be the current position of the selected peg. (!!!)
                    PegJumpAnimationFrames = PegJumpAnimation(RectArray[SpriteInfo.RectNumber].x, RectArray[SpriteInfo.RectNumber].y);
                }

                // Run through the frames of the animation
                if(GameTick < PegJumpAnimationFrames.size())
                {   
                    // Rerender the background, so the top most pegs don't get "burned in" on the screen.
                    SDL_SetRenderDrawColor(renderer, 30, 50, 100, 255);
                    SDL_RenderClear(renderer);

                    // Update RectArray to do animation [2] should be a variable: int SelectPeg (!!!)
                    RectArray[SpriteInfo.RectNumber] = {PegJumpAnimationFrames[GameTick][0], PegJumpAnimationFrames[GameTick][1], int(WIDTH/22.1), int(WIDTH/22.1*2.07)};

                    // Render the animation
                    RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
                    SDL_RenderPresent(renderer);
                }
                else
                {
                    // Reset bools
                    IsAnimationActive = false;
                    IsJumpPositionSelected = false;

                    // Stop timing (DEBUG)
                    time_point<steady_clock> end = steady_clock::now();
                    duration<double, nano> fp_ns = end - start; 
                    cout << fp_ns.count() << " ns" << endl;

                    // Clear the Stored Animation frames, so a new animation, with a new position can be used.
                    PegJumpAnimationFrames.clear();
                }

                GameTick++;
            }
        }

        // To not spam the rendering funtion, check if no peg is selected, 
        // this will only happen once since the SpriteInfo.RectNumber is set to -2.
        if(SpriteInfo.RectNumber == -1)
        {   
            if(OutlineTick != 0)
            {   
                OutlineTick = 0;
            }

            RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
            SDL_RenderPresent(renderer);

            SpriteInfo.IsSelected = false;
            SpriteInfo.RectNumber = -2;
            IsJumpPositionSelected = false;
        }
        
        // Set Gametick to zero again so the next event or animation can happen
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

