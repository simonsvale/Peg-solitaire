#ifndef animations_h_
#define animations_h_

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

// The Parabola jump
vector<vector<int> > PegJumpAnimation(int PosX, int PosY)
{
    vector<vector<int> > ReturnVector;
    vector<vector<int> > Coords;

    /* Function definition:
       Coords[1] = sqrt(128*XValue-pow(XValue, 2));

       Using a lookup table instead because calculating the squareroot 129x each time a peg moves is expensive.
    */
    Coords = {{0, 0}, {1, 11}, {2, 15}, {3, 19}, {4, 22}, {5, 24}, {6, 27}, {7, 29}, 
              {8, 30}, {9, 32}, {10, 34}, {11, 35}, {12, 37}, {13, 38}, {14, 39}, {15, 41}, 
              {16, 42}, {17, 43}, {18, 44}, {19, 45}, {20, 46}, {21, 47}, {22, 48}, {23, 49}, 
              {24, 49}, {25, 50}, {26, 51}, {27, 52}, {28, 52}, {29, 53}, {30, 54}, {31, 54}, 
              {32, 55}, {33, 55}, {34, 56}, {35, 57}, {36, 57}, {37, 58}, {38, 58}, {39, 58}, 
              {40, 59}, {41, 59}, {42, 60}, {43, 60}, {44, 60}, {45, 61}, {46, 61}, {47, 61}, 
              {48, 61}, {49, 62}, {50, 62}, {51, 62}, {52, 62}, {53, 63}, {54, 63}, {55, 63}, 
              {56, 63}, {57, 63}, {58, 63}, {59, 63}, {60, 63}, {61, 63}, {62, 63}, {63, 63}, 
              {64, 64}, {65, 63}, {66, 63}, {67, 63}, {68, 63}, {69, 63}, {70, 63}, {71, 63}, 
              {72, 63}, {73, 63}, {74, 63}, {75, 63}, {76, 62}, {77, 62}, {78, 62}, {79, 62}, 
              {80, 61}, {81, 61}, {82, 61}, {83, 61}, {84, 60}, {85, 60}, {86, 60}, {87, 59}, 
              {88, 59}, {89, 58}, {90, 58}, {91, 58}, {92, 57}, {93, 57}, {94, 56}, {95, 55}, 
              {96, 55}, {97, 54}, {98, 54}, {99, 53}, {100, 52}, {101, 52}, {102, 51}, {103, 50}, 
              {104, 49}, {105, 49}, {106, 48}, {107, 47}, {108, 46}, {109, 45}, {110, 44}, {111, 43}, 
              {112, 42}, {113, 41}, {114, 39}, {115, 38}, {116, 37}, {117, 35}, {118, 34}, {119, 32}, 
              {120, 30}, {121, 29}, {122, 27}, {123, 24}, {124, 22}, {125, 19}, {126, 15}, {127, 11}, {128, 0}};
    
    // Iterate and add the new X and Y values of the current position of the texture.
    for(int XValue = 0; XValue < 129;)
    {   
        vector<int> Vector;

        Vector.push_back(Coords[XValue][0]+PosX);
        Vector.push_back(Coords[XValue][1]+PosY);

        ReturnVector.push_back(Vector);

        cout << Vector[0] << Vector[1] << "size: " << Coords.size() << "XValue: " << XValue << endl;

        XValue++;
    }

    return ReturnVector;
}
#endif