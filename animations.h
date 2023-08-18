#ifndef animations_h_
#define animations_h_
#include <vector>
#include <cmath>

using namespace std;

// The Parabola jump
vector<vector<int> > PegJumpAnimation(int PosX, int PosY)
{
    vector<vector<int> > ReturnVector;
    vector<int> Coords = {0,0};

    for(int XValue = 0; XValue < 400;)
    {   
        vector<int> Vector;
 
        // Function definition 
        Coords[1] = sqrt(128*XValue-pow(XValue, 2));

        Vector.push_back(XValue+PosX);
        Vector.push_back(Coords[1]+PosY);

        ReturnVector.push_back(Vector);
        
        if((0 >= Coords[1]) && (XValue != 0))
        {
            break;
        }

        XValue++;

    }

    return ReturnVector;
}
#endif