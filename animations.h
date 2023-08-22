#ifndef animations_h_
#define animations_h_

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

// The Parabola jump, JumpDirection, 0 = right, 1 = left, 2 = up, 3 = down.
vector<vector<int> > PegJumpAnimation(int PosX, int PosY, int JumpLength, int JumpDirection)
{
    vector<vector<int> > ReturnVector;
    vector<int> Coords;
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
#endif