#ifndef animations_h_
#define animations_h_

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

// Function utilizing a lookup table to limit calculations using squareroot.
vector<vector<int> > PegJumpAnimation(int PosX, int PosY)
{
    vector<vector<int> > ReturnVector;
    vector<vector<int> > Coords;

    /* Function definition:
       Coords[1] = sqrt(128*XValue-pow(XValue, 2));

       Using a lookup table instead because calculating the squareroot 129x each time a peg moves is expensive.
    */
    Coords = {{0, 0}, {1, 16}, {2, 22}, {3, 27}, {4, 31}, {5, 35}, {6, 38}, {7, 41}, 
              {8, 44}, {9, 47}, {10, 49}, {11, 52}, {12, 54}, {13, 56}, {14, 58}, {15, 60}, 
              {16, 62}, {17, 63}, {18, 65}, {19, 67}, {20, 68}, {21, 70}, {22, 71}, {23, 73}, 
              {24, 74}, {25, 76}, {26, 77}, {27, 78}, {28, 80}, {29, 81}, {30, 82}, {31, 83}, 
              {32, 84}, {33, 85}, {34, 87}, {35, 88}, {36, 89}, {37, 90}, {38, 91}, {39, 92}, 
              {40, 93}, {41, 94}, {42, 95}, {43, 95}, {44, 96}, {45, 97}, {46, 98}, {47, 99}, 
              {48, 100}, {49, 100}, {50, 101}, {51, 102}, {52, 103}, {53, 103}, {54, 104}, {55, 105}, 
              {56, 106}, {57, 106}, {58, 107}, {59, 108}, {60, 108}, {61, 109}, {62, 109}, {63, 110}, 
              {64, 111}, {65, 111}, {66, 112}, {67, 112}, {68, 113}, {69, 113}, {70, 114}, {71, 114}, 
              {72, 115}, {73, 115}, {74, 116}, {75, 116}, {76, 117}, {77, 117}, {78, 118}, {79, 118}, 
              {80, 118}, {81, 119}, {82, 119}, {83, 120}, {84, 120}, {85, 120}, {86, 121}, {87, 121}, 
              {88, 121}, {89, 122}, {90, 122}, {91, 122}, {92, 123}, {93, 123}, {94, 123}, {95, 124}, 
              {96, 124}, {97, 124}, {98, 124}, {99, 125}, {100, 125}, {101, 125}, {102, 125}, {103, 125}, 
              {104, 126}, {105, 126}, {106, 126}, {107, 126}, {108, 126}, {109, 127}, {110, 127}, {111, 127}, 
              {112, 127}, {113, 127}, {114, 127}, {115, 127}, {116, 127}, {117, 127}, {118, 128}, {119, 128}, 
              {120, 128}, {121, 128}, {122, 128}, {123, 128}, {124, 128}, {125, 128}, {126, 128}, {127, 128}, 
              {128, 128}, {129, 128}, {130, 128}, {131, 128}, {132, 128}, {133, 128}, {134, 128}, {135, 128}, 
              {136, 128}, {137, 128}, {138, 128}, {139, 128}, {140, 127}, {141, 127}, {142, 127}, {143, 127}, 
              {144, 127}, {145, 127}, {146, 127}, {147, 127}, {148, 127}, {149, 126}, {150, 126}, {151, 126}, 
              {152, 126}, {153, 126}, {154, 125}, {155, 125}, {156, 125}, {157, 125}, {158, 125}, {159, 124}, 
              {160, 124}, {161, 124}, {162, 124}, {163, 123}, {164, 123}, {165, 123}, {166, 122}, {167, 122}, 
              {168, 122}, {169, 121}, {170, 121}, {171, 121}, {172, 120}, {173, 120}, {174, 120}, {175, 119}, 
              {176, 119}, {177, 118}, {178, 118}, {179, 118}, {180, 117}, {181, 117}, {182, 116}, {183, 116}, 
              {184, 115}, {185, 115}, {186, 114}, {187, 114}, {188, 113}, {189, 113}, {190, 112}, {191, 112}, 
              {192, 111}, {193, 111}, {194, 110}, {195, 109}, {196, 109}, {197, 108}, {198, 108}, {199, 107}, 
              {200, 106}, {201, 106}, {202, 105}, {203, 104}, {204, 103}, {205, 103}, {206, 102}, {207, 101}, 
              {208, 100}, {209, 100}, {210, 99}, {211, 98}, {212, 97}, {213, 96}, {214, 95}, {215, 95}, {216, 94}, 
              {217, 93}, {218, 92}, {219, 91}, {220, 90}, {221, 89}, {222, 88}, {223, 87}, {224, 85}, {225, 84}, 
              {226, 83}, {227, 82}, {228, 81}, {229, 80}, {230, 78}, {231, 77}, {232, 76}, {233, 74}, {234, 73}, 
              {235, 71}, {236, 70}, {237, 68}, {238, 67}, {239, 65}, {240, 63}, {241, 62}, {242, 60}, {243, 58}, 
              {244, 56}, {245, 54}, {246, 52}, {247, 49}, {248, 47}, {249, 44}, {250, 41}, {251, 38}, {252, 35}, 
              {253, 31}, {254, 27}, {255, 22}, {256, 16}, {257, 0}};
    
    // Iterate and add the new X and Y values of the current position of the texture.
    for(int XValue = 0; XValue < Coords.size();)
    {   
        vector<int> Vector;

        Vector.push_back(Coords[XValue][0]+PosX);
        Vector.push_back(Coords[XValue][1]+PosY);

        ReturnVector.push_back(Vector);

        XValue++;
    }

    return ReturnVector;
}

// Function utilizing calculations to determine the position of the sprite each animation frame. 
// Using the lookup table is faster than recalculating each animation (probably beacause of the sqrt() calculation)
vector<vector<int> > PegJumpAnimationCalculated(int PosX, int PosY, int JumpLength)
{
    vector<vector<int> > ReturnVector;
    vector<int> Vector;

    // Iterate and add the new X and Y values of the current position of the texture.
    for(int XValue = 0; XValue < JumpLength+1;)
    {   
        Vector.push_back(XValue+PosX);
        Vector.push_back(int(sqrt(JumpLength*XValue-pow(XValue, 2)))+PosY);

        ReturnVector.push_back(Vector);
        Vector.clear();

        XValue++;
    }

    return ReturnVector;
}


// Function for creating lookup tables 
void LookupTableCalculator(int JumpLength)
{
    vector<int> Vector;
    
    // First line:
    cout << "vector<vector<int> > Table = {";

    // Iterate and add the new X and Y values of the current position of the texture.
    for(int XValue = 0; XValue < JumpLength+1;)
    {   
        Vector.push_back(XValue);
        Vector.push_back(int(sqrt(JumpLength*XValue-pow(XValue, 2))));

        // All coords
        cout << "{" << Vector[0] << ", " << Vector[1] << "}, ";

        Vector.clear();

        XValue++;
    }

    cout << "};" << endl;
}

#endif