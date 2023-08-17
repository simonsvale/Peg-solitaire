#ifndef animations_h_
#define animations_h_
#include <vector>

using namespace std;

// The Parabola jump
vector<vector<int> > PegJumpAnimation(int PosX, int PosY)
{
    vector<vector<int> > ReturnVector;
    vector<int> Coords = {0,0};

    for(int XValue = 0; XValue < 200;)
    {   
        vector<int> Vector;
 
        // Function definition
        Coords[1] = -(XValue*XValue)+100*XValue;

        Vector.push_back(XValue+PosX);
        Vector.push_back(Coords[1]+PosY);

        ReturnVector.push_back(Vector);
        
        cout << XValue << ": " << ReturnVector[XValue][0] << ", " << ReturnVector[XValue][1] << endl;

        if((0 >= Coords[1]) && (XValue != 0))
        {
            break;
        }

        XValue++;

    }

    return ReturnVector;
}
#endif