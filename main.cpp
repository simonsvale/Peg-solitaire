// Compilation settings
// g++ -O -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows

// Include necessary built-in header files
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <windows.h> 

// Include non built-in header files
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Include own header file(s)
#include "animations.h"

using namespace std;

// Const char for holding the position of the textures.
const char *ImagePathArray[] = {"textures/dummy.png", "textures/PegBoard.png", "textures/Peg.png", "textures/HoleSelect.png", "textures/PegOutline.png"};
const int ImagePathArraySize = (*(&ImagePathArray + 1) - ImagePathArray);

// Struct for clicked peg return
struct ClickedSprite
{
    int RectNumber;
    bool IsSelected;
    bool IsPeg;
};

struct PossibleMoves
{
    bool North;
    bool South;
    bool East;
    bool West;
};

struct UpdateBoard
{
    vector<int> NewBoardPosition;
    int JumpedPegRectNumber;
};

struct RectSize
{
    int WIDTH;
    int HEIGHT;

};

struct UndoMoveInfo
{
    int PegRect;
    int RemovedPegRect;
    int MoveDirection;
};

struct UndoReturnInfo
{
    vector<int> BoardLayout;
    vector<SDL_Rect> RectArr;
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
void RenderEverything(SDL_Renderer *renderer, SDL_Texture *TextureArr[], vector<SDL_Rect> RectArr, vector<int> TextureAmountArr, ClickedSprite Peg, int ArraySum)
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
ClickedSprite SpriteClickDetection(SDL_Point MousePos, vector<SDL_Rect> RectArray, vector<int> RectRange)
{
    // Struct for holding IsSelected (bool) and RectNumber (int)
    ClickedSprite returnVal;

    for(int RectNumber = RectRange[0]; RectNumber < RectRange[1];)
    {   
        // Take mouse pos and see if it is inside the boundary of any peg sprite.
        if(SDL_PointInRect(&MousePos, &RectArray[RectNumber]) == true)
        {
            // If is is set the structs members to the correct information.
            returnVal.IsSelected = true;
            returnVal.RectNumber = RectNumber;
            
            if(RectNumber < 34)
            {
                returnVal.IsPeg = true;
            }
            else
            {
                returnVal.IsPeg = false;
            }

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
PossibleMoves GetPossibleMoves(int PegPosition, vector<int> CurrentBoardLayout)
{
    // define return struct
    PossibleMoves Moves; Moves.North = false; Moves.South = false; Moves.East = false; Moves.West = false;

    const int BoardSize = CurrentBoardLayout.size();

    // Vectors for position checks   North,         South,         East,          West
    vector<int> CheckPegLocations = {PegPosition-7, PegPosition+7, PegPosition+1, PegPosition-1};
    vector<int> CheckEmptyLocations = {PegPosition-14, PegPosition+14, PegPosition+2, PegPosition-2};

    for(int Count = 0; Count < 4;)
    {   
    
        // Check if there is a peg on each side of the selected peg
        if((CurrentBoardLayout[CheckPegLocations[Count]] != 0) && (CurrentBoardLayout[CheckPegLocations[Count]] != -1))
        {      
            // North
            if(Count == 0)
            {
                // If there is check if there is an empty space beside that peg.
                if((CurrentBoardLayout[CheckEmptyLocations[Count]] == 0) && ((PegPosition-14) >= 0))
                {
                    Moves.North = true;
                }
                else
                {
                    Moves.North = false;
                }

            }
            // South
            else if(Count == 1)
            {

                // If there is check if there is an empty space beside that peg.
                if((CurrentBoardLayout[CheckEmptyLocations[Count]] == 0) && ((PegPosition+14) <= BoardSize))
                {
                    Moves.South = true;
                }
                else
                {
                    Moves.South = false;
                }

            }
            // East
            else if(Count == 2)
            {
                // Prevent pins from jumping in non cardinal directions.
                if( (((PegPosition+1) % 7) != 0) && (((PegPosition+2) % 7) != 0))
                {
                    // If there is check if there is an empty space beside that peg.
                    if(CurrentBoardLayout[CheckEmptyLocations[Count]] == 0)
                    {
                        Moves.East = true;
                    }
                    else
                    {
                        Moves.East = false;
                    }
                }
            }
            // West
            else if(Count == 3)
            {   
                if( ((PegPosition % 7) != 0) && (((PegPosition-1) % 7) != 0))
                {
                    // If there is check if there is an empty space beside that peg.
                    if(CurrentBoardLayout[CheckEmptyLocations[Count]] == 0)
                    {
                        Moves.West = true;
                    }
                    else
                    {
                        Moves.West = false;
                    }
                }
            }
        }

        Count++;
    }

    return Moves;
}

// Function for setting the position of hole textures when a peg have been selected.
vector<SDL_Rect> SetHoleRect(int RelativeHolePosition, int RectIndex, vector<SDL_Rect> RectArray, int TrueBoardIndexPosition, RectSize Hole, vector<int> PegSetupX, vector<int> PegSetupY, int DisplaySizeW)
{
    int BoardPosX;
    int BoardPosY;

    // Convert the Indexed Rect position for the select hole, to cartesian coordinates in the 7x7 grid.
    BoardPosY = floor((TrueBoardIndexPosition+(RelativeHolePosition))/7)+1;
    BoardPosX = TrueBoardIndexPosition+(RelativeHolePosition) - (BoardPosY-1)*7+1;

    RectArray[RectIndex] = {PegSetupX[BoardPosX-1], (PegSetupY[BoardPosY-1]+(DisplaySizeW / (1920/55))), Hole.WIDTH, Hole.HEIGHT};

    return RectArray;
}

// A function for converting the static RectNumber of the textures which does not change when a peg moves,
// to the actual board position of that peg.
int GetRectBoardPosition(int RectNumber, vector<int> BoardLayout)
{
    // For loop iterating through all positions
    // Note: This can be made more efficient since 14 more positions are not used.
    for(int Index = 2; Index < 49;)
    {   
        // Since the RectNumber is predefined, look for it.
        if(RectNumber == BoardLayout[Index])
        {   
            // Return the real board position of the texture
            return Index;
        }   
        Index++;
    }

    // Return fail
    return -1;
}

// Function for updating the board with new peg positions.
UpdateBoard UpdateBoardPosition(vector<int> CurrentBoardLayout, int RelativeHolePosition, ClickedSprite SpriteInfoPeg, int TruePegPosition)
{
    UpdateBoard NewBoardLayout;

    NewBoardLayout.NewBoardPosition = CurrentBoardLayout;

    // Set New position
    NewBoardLayout.NewBoardPosition[TruePegPosition + RelativeHolePosition] = SpriteInfoPeg.RectNumber;

    // Remove the "jumped" peg
    int RelativeJumpedPegPos = TruePegPosition + (RelativeHolePosition/2);

    NewBoardLayout.JumpedPegRectNumber = NewBoardLayout.NewBoardPosition[RelativeJumpedPegPos];
    NewBoardLayout.NewBoardPosition[RelativeJumpedPegPos] = 0;

    return NewBoardLayout;
}   


// Convert index number in 7x7 grid to cartesian coordiantes
vector<int> IndexToCartesian(int Position)
{
    vector<int> Coords;

    int Y= floor(Position/7)+1;
    int X = Position - (Y-1)*7+1;

    Coords.push_back(X);
    Coords.push_back(Y);

    return Coords;
}


// Function for undoing moves.
UndoReturnInfo UndoPegMove(vector<int> CurrentBoardLayout, UndoMoveInfo PegMove, vector<SDL_Rect> RectArr, vector<int> PegSetupX, vector<int> PegSetupY, RectSize Peg)
{
    // setup return rect
    UndoReturnInfo ReturnStruct;

    // Get the true position of the peg.
    int TruePegPos = GetRectBoardPosition(PegMove.PegRect, CurrentBoardLayout);
    int TrueRemovedPegPos = TruePegPos;

    if(PegMove.MoveDirection == 0)
    {   
        CurrentBoardLayout[TruePegPos+14] = PegMove.PegRect;
        CurrentBoardLayout[TruePegPos+7] = PegMove.RemovedPegRect;
        CurrentBoardLayout[TruePegPos] = 0;

        TrueRemovedPegPos += 7;
        TruePegPos += 14;

        // Set peg's rect to the previous position and add back the removed peg.
        vector<int> TruePegCoords = IndexToCartesian(TruePegPos);
        vector<int> TrueRemovedPegCoords = IndexToCartesian(TrueRemovedPegPos);
        
        // Update Rects to old positions
        RectArr[PegMove.RemovedPegRect] = {int(PegSetupX[TrueRemovedPegCoords[0]-1]), int(PegSetupY[TrueRemovedPegCoords[1]-1]), Peg.WIDTH, Peg.HEIGHT};
        RectArr[PegMove.PegRect] = {int(PegSetupX[TruePegCoords[0]-1]), int(PegSetupY[TruePegCoords[1]-1]), Peg.WIDTH, Peg.HEIGHT};
  
        // Set new Boardlayout
        ReturnStruct.BoardLayout = CurrentBoardLayout;
        ReturnStruct.RectArr = RectArr;

        return ReturnStruct;
    }

    if(PegMove.MoveDirection == 1)
    {   
        CurrentBoardLayout[TruePegPos-14] = PegMove.PegRect;
        CurrentBoardLayout[TruePegPos-7] = PegMove.RemovedPegRect;
        CurrentBoardLayout[TruePegPos] = 0;

        TrueRemovedPegPos -= 7;
        TruePegPos -= 14;

        // Set peg's rect to the previous position and add back the removed peg.
        vector<int> TruePegCoords = IndexToCartesian(TruePegPos);
        vector<int> TrueRemovedPegCoords = IndexToCartesian(TrueRemovedPegPos);

        // Update Rects to old positions
        RectArr[PegMove.RemovedPegRect] = {int(PegSetupX[TrueRemovedPegCoords[0]-1]), int(PegSetupY[TrueRemovedPegCoords[1]-1]), Peg.WIDTH, Peg.HEIGHT};
        RectArr[PegMove.PegRect] = {int(PegSetupX[TruePegCoords[0]-1]), int(PegSetupY[TruePegCoords[1]-1]), Peg.WIDTH, Peg.HEIGHT};

        // Set new Boardlayout
        ReturnStruct.BoardLayout = CurrentBoardLayout;
        ReturnStruct.RectArr = RectArr;

        return ReturnStruct;
    }

    if(PegMove.MoveDirection == 2)
    {
        CurrentBoardLayout[TruePegPos-2] = PegMove.PegRect;
        CurrentBoardLayout[TruePegPos-1] = PegMove.RemovedPegRect;
        CurrentBoardLayout[TruePegPos] = 0;

        TrueRemovedPegPos -= 1;
        TruePegPos -= 2;

        // Set peg's rect to the previous position and add back the removed peg.
        vector<int> TruePegCoords = IndexToCartesian(TruePegPos);
        vector<int> TrueRemovedPegCoords = IndexToCartesian(TrueRemovedPegPos);

        // Update Rects to old positions
        RectArr[PegMove.RemovedPegRect] = {int(PegSetupX[TrueRemovedPegCoords[0]-1]), int(PegSetupY[TrueRemovedPegCoords[1]-1]), Peg.WIDTH, Peg.HEIGHT};
        RectArr[PegMove.PegRect] = {int(PegSetupX[TruePegCoords[0]-1]), int(PegSetupY[TruePegCoords[1]-1]), Peg.WIDTH, Peg.HEIGHT};
        
        // Set new Boardlayout
        ReturnStruct.BoardLayout = CurrentBoardLayout;
        ReturnStruct.RectArr = RectArr;

        return ReturnStruct;
    }

    if(PegMove.MoveDirection == 3)
    {
        CurrentBoardLayout[TruePegPos+2] = PegMove.PegRect;
        CurrentBoardLayout[TruePegPos+1] = PegMove.RemovedPegRect;
        CurrentBoardLayout[TruePegPos] = 0;

        TrueRemovedPegPos += 1;
        TruePegPos += 2;

        // Set peg's rect to the previous position and add back the removed peg.
        vector<int> TruePegCoords = IndexToCartesian(TruePegPos);
        vector<int> TrueRemovedPegCoords = IndexToCartesian(TrueRemovedPegPos);

        // Update Rects to old positions
        RectArr[PegMove.RemovedPegRect] = {int(PegSetupX[TrueRemovedPegCoords[0]-1]), int(PegSetupY[TrueRemovedPegCoords[1]-1]), Peg.WIDTH, Peg.HEIGHT};
        RectArr[PegMove.PegRect] = {int(PegSetupX[TruePegCoords[0]-1]), int(PegSetupY[TruePegCoords[1]-1]), Peg.WIDTH, Peg.HEIGHT};
        
        // Set new Boardlayout
        ReturnStruct.BoardLayout = CurrentBoardLayout;
        ReturnStruct.RectArr = RectArr;

        return ReturnStruct;
    }

    // just return the original board if no moves where made.
    return ReturnStruct;
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
    
    UndoMoveInfo UndoStruct;
    vector<UndoMoveInfo> UndoVector;


    UndoReturnInfo Undo;

    // Reset variables
    int ResetPegValue;

    // variables for conversion from Rect number to cartesian coordinates.
    int ResetPegPosX;
    int ResetPegPosY;

    // Rects corrosponding to distination hole directions.
    int NORTH = 34;
    int SOUTH = 35;
    int EAST = 36;
    int WEST = 37;

    // For storing the actual images, that are used as textures.
    SDL_Surface *SurfaceArray[ImagePathArraySize];
    SDL_Texture *TextureArray[ImagePathArraySize];

    SDL_Window *window;
    SDL_Renderer *renderer; 

    int WIDTH;
    int HEIGHT;

    RectSize Peg;
    RectSize Hole;
    RectSize BoardSize;

    // initialize SDL window and renderer
    SDL_Init(SDL_INIT_EVERYTHING);

    // Throws an error in the VS IDE, but somehow it still works...
    SDL_DisplayMode DisplaySize;
    SDL_GetCurrentDisplayMode(0, &DisplaySize);

    // Use the width and height of the screen to make the window compatible with all screen resolutions.
    WIDTH = DisplaySize.w/2;
    HEIGHT = DisplaySize.h/1.4;

    Peg.WIDTH = int(WIDTH / 22.1);
    Peg.HEIGHT = int(WIDTH / 22.1 * 2.07);

    Hole.WIDTH = int(WIDTH / 22.1);
    Hole.HEIGHT = int(((WIDTH * 1.1) / 22.1) / 1.25);

    BoardSize.WIDTH = int(WIDTH);

    // Create the SDL window and set its title
    SDL_CreateWindowAndRenderer(WIDTH, WIDTH/1.2, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
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

    // All 33 positions, SpriteInfo.RectNumber = populated, 0 = empty, -1 = Null.
    vector<int> BoardLayout = {-1, -1, 2, 3, 4, -1, -1,
                               -1, -1, 5, 6, 7, -1, -1,
                                 8, 9, 10, 11, 12, 13, 14, 
                                 15, 16, 17, 0, 18, 19, 20, 
                                 21, 22, 23, 24, 25, 26, 27, 
                               -1, -1, 28, 29, 30, -1, -1,
                               -1, -1, 31, 32, 33, -1, -1};

    UpdateBoard Board;

    // Struct for storing possible moves when selecting a peg. 
    //                               North, South, East,  West
    PossibleMoves SelectedPegMoves;
    int TruePegPosition;

    // Vector for storing Rects.
    vector<SDL_Rect> RectArray;

    // Ranges of specific rects
    vector<int> PegRectRange = {2, 38};

    // Push Rects to the RectArray
    RectArray.push_back({-100, -100, 90, 90}); // Dummy texture rect
    RectArray.push_back({0, -(DisplaySize.w/24), BoardSize.WIDTH, BoardSize.WIDTH}); // board texture rect

    // Setup positions
    vector<int> PegSetupX = {int(DisplaySize.w / (1920/73.0)), int(DisplaySize.w / (960/101.0)), int(DisplaySize.w / (64/11.0)), 
                             int(DisplaySize.w / (960/229.0)), int(DisplaySize.w / (1920/587.0)), int(DisplaySize.w / (384/143.0)), int(DisplaySize.w / (640/281.0))};

    vector<int> PegSetupY = {int(DisplaySize.w / 480), int(DisplaySize.w / (960/47.0)), int(DisplaySize.w / (240/23.0)), 
                             int(DisplaySize.w / (960/139.0)), int(DisplaySize.w / (1920/371.0)), int(DisplaySize.w / (1920/461.0)), int(DisplaySize.w / (1920/551.0))};

    // Create the board of 7x7 pegs
    for(int PegPosY = 0; PegPosY < 7;)
    {
        for(int PegPosX = 0; PegPosX < 7;)
        {
            RectArray.push_back({PegSetupX[PegPosX], PegSetupY[PegPosY], Peg.WIDTH, Peg.HEIGHT});

            PegPosX++;
        }

        PegPosY++;
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
    for(int RectCount = 0; RectCount < 4;)
    {
        RectArray.push_back({-100, -100, Hole.WIDTH, Hole.HEIGHT});
        RectCount++;
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

    // Setup Struct for a sprite
    ClickedSprite SpriteInfo; SpriteInfo.IsSelected = false; SpriteInfo.RectNumber = -1; SpriteInfo.IsPeg = false;
    ClickedSprite SpriteInfoPeg; SpriteInfo.IsSelected = false; SpriteInfo.RectNumber = -1;

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
                SDL_RenderClear(renderer);
            }
        }

        if (SDL_PollEvent(&windowEvent))
        {
            if (SDL_QUIT == windowEvent.type)
            {
                break;
            }

            // Listen for r press, to reset the game.
            if ((IsAnimationActive == false) && (SDL_KEYUP == windowEvent.type))
			{   
                if(windowEvent.key.keysym.sym == SDLK_r)
                {
                    // Reset board
                    BoardLayout = {-1, -1, 2, 3, 4, -1, -1,
                                -1, -1, 5, 6, 7, -1, -1,
                                    8, 9, 10, 11, 12, 13, 14, 
                                    15, 16, 17, 0, 18, 19, 20, 
                                    21, 22, 23, 24, 25, 26, 27, 
                                -1, -1, 28, 29, 30, -1, -1,
                                -1, -1, 31, 32, 33, -1, -1};

                    // Reset peg rect positions
                    for(int IndexedResetPeg = 2; IndexedResetPeg < 49;)
                    {      
                        // Get Rect Number from BoardLayout
                        ResetPegValue = BoardLayout[IndexedResetPeg];
                        
                        // Check if the indexResetPeg is an actual peg position
                        if((ResetPegValue != -1) && (ResetPegValue != 0))
                        {   
                            // Convert IndexedResetPeg to cartesian coordinates
                            ResetPegPosY = floor(IndexedResetPeg/7)+1;
                            ResetPegPosX = IndexedResetPeg - (ResetPegPosY-1)*7+1;

                            // Set New Peg Rect Position
                            RectArray[ResetPegValue] = {PegSetupX[ResetPegPosX-1], PegSetupY[ResetPegPosY-1], Peg.WIDTH, Peg.HEIGHT};
                        }
                        IndexedResetPeg++;
                    }

                    // Rerender screen
                    RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
                    SDL_RenderPresent(renderer);
                    SDL_RenderClear(renderer);
                }

                // Check for z
                if(windowEvent.key.keysym.sym == SDLK_z)
                {   
                    if(UndoVector.size() > 0)
                    {
                        // Undo last move
                        Undo = UndoPegMove(BoardLayout, UndoVector.back(), RectArray, PegSetupX, PegSetupY, Peg);

                        BoardLayout = Undo.BoardLayout;
                        RectArray = Undo.RectArr;

                        // Pop the last undo off the vector
                        UndoVector.pop_back();

                        // Render selected peg with outline and the possible moves.
                        RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
                        SDL_RenderPresent(renderer);
                        SDL_RenderClear(renderer);
                    }
                }

            }

            if ((SDL_MOUSEBUTTONDOWN == windowEvent.type) && (IsAnimationActive == false))
			{  
                // Detect if any sprite was clicked/selected, and return a struct with that information.
                SpriteInfo = SpriteClickDetection(MousePos, RectArray, PegRectRange);

                if(SpriteInfo.IsPeg == true)
                {
                    SpriteInfoPeg = SpriteInfo;
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

                    PreviousRectNumber.clear();
                    PreviousRectNumber.push_back(SpriteInfo.RectNumber);

                    // Reset selection holes position if a hole have not been selected
                    if(SpriteInfo.RectNumber < 34)
                    {
                        for(int HoleNumber = 34; HoleNumber < 38;)
                        {
                            RectArray[HoleNumber] = {-100, -100, Hole.WIDTH, Hole.HEIGHT};
                            HoleNumber++;
                        }
                    }
                    else
                    {
                        IsJumpPositionSelected = true;
                        IsAnimationActive = true;
                    }
                }
            }
        }

        // Check if a peg have been selected
        if(SpriteInfoPeg.IsSelected == true)
        {   
            if((IsJumpPositionSelected == false) && (IsOutlineRendered == false))
            {
                IsOutlineRendered = true;

                // Get actual location of peg.
                TruePegPosition = GetRectBoardPosition(SpriteInfoPeg.RectNumber, BoardLayout);

                // Get the possible moves of that peg.
                SelectedPegMoves = GetPossibleMoves(TruePegPosition, BoardLayout);

                // Set the HoleSelect texture position
                if(SelectedPegMoves.North == true)
                {   
                    RectArray = SetHoleRect(-14, 34, RectArray, TruePegPosition, Hole, PegSetupX, PegSetupY, DisplaySize.w);
                }

                if(SelectedPegMoves.South == true)
                {
                    RectArray = SetHoleRect(14, 35, RectArray, TruePegPosition, Hole, PegSetupX, PegSetupY, DisplaySize.w);
                }

                if(SelectedPegMoves.East == true)
                {
                    RectArray = SetHoleRect(2, 36, RectArray, TruePegPosition, Hole, PegSetupX, PegSetupY, DisplaySize.w);
                }

                if(SelectedPegMoves.West == true)
                {
                    RectArray = SetHoleRect(-2, 37, RectArray, TruePegPosition, Hole, PegSetupX, PegSetupY, DisplaySize.w);
                }
                
                // Render selected peg with outline and the possible moves.
                RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
                SDL_RenderPresent(renderer);
                SDL_RenderClear(renderer);
            }
            
            // If a JumpPos have been selected then the animation
            if((IsAnimationActive == true) && (IsJumpPositionSelected == true))
            {   
                if(PegJumpAnimationFrames.size() == 0)
                {
                    // Set the peg's previous position to 0.
                    BoardLayout[TruePegPosition] = 0;

                    // Set new board position, based on chosen direction.
                    if(SpriteInfo.RectNumber == NORTH)
                    {
                        Board = UpdateBoardPosition(BoardLayout, -14, SpriteInfoPeg, TruePegPosition);
                        UndoStruct.RemovedPegRect = BoardLayout[TruePegPosition-7];
                        UndoStruct.MoveDirection = 0;
                    }

                    if(SpriteInfo.RectNumber == SOUTH)
                    {
                        Board = UpdateBoardPosition(BoardLayout, 14, SpriteInfoPeg, TruePegPosition);
                        UndoStruct.RemovedPegRect = BoardLayout[TruePegPosition+7];
                        UndoStruct.MoveDirection = 1;
                    }

                    if(SpriteInfo.RectNumber == EAST)
                    {
                        Board = UpdateBoardPosition(BoardLayout, 2, SpriteInfoPeg, TruePegPosition);
                        UndoStruct.RemovedPegRect = BoardLayout[TruePegPosition+1];
                        UndoStruct.MoveDirection = 2;
                    }

                    if(SpriteInfo.RectNumber == WEST)
                    {
                        Board = UpdateBoardPosition(BoardLayout, -2, SpriteInfoPeg, TruePegPosition);
                        UndoStruct.RemovedPegRect = BoardLayout[TruePegPosition-1];
                        UndoStruct.MoveDirection = 3;
                    }

                    UndoStruct.PegRect = SpriteInfoPeg.RectNumber;
                    
                    // Take the chosen direction SpriteInfo.RectNumber, which is NORTH, SOUTH, EAST or WEST, and run the correct animation.
                    PegJumpAnimationFrames = PegJumpAnimation(RectArray[SpriteInfoPeg.RectNumber].x, RectArray[SpriteInfoPeg.RectNumber].y, SpriteInfo.RectNumber, TruePegPosition, DisplaySize.w);

                    // Update board layout
                    BoardLayout = Board.NewBoardPosition;

                    // Reset the destination holes position.
                    for(int HoleNumber = 34; HoleNumber < 38;)
                    {   
                        RectArray[HoleNumber] = {-100, -100, Hole.WIDTH, Hole.HEIGHT};

                        HoleNumber++;
                    }

                    // Push UndoStruct onto vector to be able to undo move.
                    UndoVector.push_back(UndoStruct);

                    // Rerender the screen with the holes new position set in the top left.
                    RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
                    SDL_RenderPresent(renderer);
                }

                // Run through the frames of the animation
                if(GameTick < PegJumpAnimationFrames.size())
                {   
                    // Rerender the background, so the top most pegs don't get "burned in" on the screen.
                    SDL_SetRenderDrawColor(renderer, 30, 50, 100, 255);
                    SDL_RenderClear(renderer);

                    // Update RectArray to do animation.
                    RectArray[SpriteInfoPeg.RectNumber] = {PegJumpAnimationFrames[GameTick][0], PegJumpAnimationFrames[GameTick][1], Peg.WIDTH, Peg.HEIGHT};

                    // Render the animation
                    RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
                    SDL_RenderPresent(renderer);
                }
                else
                {
                    // Reset bools
                    IsAnimationActive = false;
                    IsJumpPositionSelected = false;

                    // Clear the Stored Animation frames, so a new animation, with a new position can be used.
                    PegJumpAnimationFrames.clear();

                    SpriteInfoPeg.IsSelected = false;
                    SpriteInfoPeg.RectNumber = -2;

                    SpriteInfo.IsSelected = false;
                    SpriteInfo.RectNumber = -2;

                    // Remove jumped peg (Could also have an animation, perhaps like when the cards bounce in normal solitaire)
                    RectArray[Board.JumpedPegRectNumber] = {-100, -100, Peg.WIDTH, Peg.HEIGHT};

                    RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
                    SDL_RenderPresent(renderer);
                    SDL_RenderClear(renderer);
                }

                GameTick++;
            }
        }

        // To not spam the rendering funtion, check if no peg is selected, 
        // this will only happen once since the SpriteInfoPeg.RectNumber is set to -2.
        if(SpriteInfo.RectNumber == -1)
        {   
            if(OutlineTick != 0)
            {   
                OutlineTick = 0;
            }
            
            // Reset selection holes position
            for(int HoleNumber = 34; HoleNumber < 38;)
            {
                RectArray[HoleNumber] = {-100, -100, Hole.WIDTH, Hole.HEIGHT};
                HoleNumber++;
            }

            RenderEverything(renderer, TextureArray, RectArray, TextureAmountArray, SpriteInfo, ArraySum);
            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);

            SpriteInfoPeg.IsSelected = false;
            SpriteInfoPeg.RectNumber = -2;

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

