#ifndef animations_h_
#define animations_h_

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

// Function utilizing a lookup table to limit calculations using squareroot.
vector<vector<int> > PegJumpAnimation(int PosX, int PosY, int Direction, int TruePegPosition, int DisplaySizeW)
{
    vector<vector<int> > ReturnVector;
    vector<vector<int> > Coords;

    /* Function definition:
        Coords[1] = sqrt(128*XValue-pow(XValue, 2));

        Using a lookup table instead because calculating the squareroot 129x each time a peg moves is expensive.
    */

    // North
    if(Direction == 34)
    {
        if((TruePegPosition > 20) && (TruePegPosition <= 27))
        {   
            // 184
            Coords = {{0, -4}, {0, -9}, {0, -14}, {0, -19}, {0, -24}, {0, -29}, {0, -34}, {0, -39}, {0, -44}, 
                      {0, -49}, {0, -54}, {0, -59}, {0, -64}, {0, -69}, {0, -74}, {0, -79}, {0, -84}, {0, -89}, 
                      {0, -94}, {0, -99}, {0, -104}, {0, -109}, {0, -114}, {0, -119}, {0, -124}, {0, -129}, 
                      {0, -134}, {0, -139}, {0, -144}, {0, -149}, {0, -154}, {0, -159}, {0, -164}, {0, -169}, 
                      {0, -174}, {0, -179}, {0, -184}};
        }

        if((TruePegPosition > 27) && (TruePegPosition <= 34))
        {
            // 187
            Coords = {{0, -4}, {0, -9}, {0, -14}, {0, -19}, {0, -24}, {0, -29}, {0, -34}, {0, -39}, {0, -44}, 
                      {0, -49}, {0, -54}, {0, -59}, {0, -64}, {0, -69}, {0, -74}, {0, -79}, {0, -84}, {0, -89}, 
                      {0, -94}, {0, -99}, {0, -104}, {0, -109}, {0, -114}, {0, -119}, {0, -124}, {0, -129}, 
                      {0, -134}, {0, -139}, {0, -144}, {0, -149}, {0, -154}, {0, -159}, {0, -164}, {0, -169}, 
                      {0, -174}, {0, -179}, {0, -184}, {0, -187}};
        }

        if((TruePegPosition > 34) && (TruePegPosition <= 41))
        {
            // 183
            Coords = {{0, -4}, {0, -9}, {0, -14}, {0, -19}, {0, -24}, {0, -29}, {0, -34}, {0, -39}, {0, -44}, 
                      {0, -49}, {0, -54}, {0, -59}, {0, -64}, {0, -69}, {0, -74}, {0, -79}, {0, -84}, {0, -89}, 
                      {0, -94}, {0, -99}, {0, -104}, {0, -109}, {0, -114}, {0, -119}, {0, -124}, {0, -129}, 
                      {0, -134}, {0, -139}, {0, -144}, {0, -149}, {0, -154}, {0, -159}, {0, -164}, {0, -169}, 
                      {0, -174}, {0, -179}, {0, -183}};
        }

        if(((TruePegPosition > 41) && (TruePegPosition <= 48)) || ((TruePegPosition > 13) && (TruePegPosition <= 20)))
        {
            // 180
            Coords = {{0, -4}, {0, -9}, {0, -14}, {0, -19}, {0, -24}, {0, -29}, {0, -34}, {0, -39}, {0, -44}, 
                      {0, -49}, {0, -54}, {0, -59}, {0, -64}, {0, -69}, {0, -74}, {0, -79}, {0, -84}, {0, -89}, 
                      {0, -94}, {0, -99}, {0, -104}, {0, -109}, {0, -114}, {0, -119}, {0, -124}, {0, -129}, 
                      {0, -134}, {0, -139}, {0, -144}, {0, -149}, {0, -154}, {0, -159}, {0, -164}, {0, -169}, 
                      {0, -174}, {0, -180}};
        }
    }


    // South
    if((Direction == 35))
    {   
        if((TruePegPosition <= 6) || ((TruePegPosition > 27) && (TruePegPosition <= 34)))
        {   
            // 180 pixel jump
            Coords = {{0, 4}, {0, 9}, {0, 14}, {0, 19}, {0, 24}, {0, 29}, {0, 34}, {0, 39}, {0, 44}, 
                      {0, 49}, {0, 54}, {0, 59}, {0, 64}, {0, 69}, {0, 74}, {0, 79}, {0, 84}, {0, 89}, 
                      {0, 94}, {0, 99}, {0, 104}, {0, 109}, {0, 114}, {0, 119}, {0, 124}, {0, 129}, 
                      {0, 134}, {0, 139}, {0, 144}, {0, 149}, {0, 154}, {0, 159}, {0, 164}, {0, 169}, 
                      {0, 174}, {0, 180}};
        }

        else if((TruePegPosition > 6) && (TruePegPosition <= 13))
        {
            // 184 pixel jump
            Coords = {{0, 4}, {0, 9}, {0, 14}, {0, 19}, {0, 24}, {0, 29}, {0, 34}, {0, 39}, {0, 44}, 
                      {0, 49}, {0, 54}, {0, 59}, {0, 64}, {0, 69}, {0, 74}, {0, 79}, {0, 84}, {0, 89}, 
                      {0, 94}, {0, 99}, {0, 104}, {0, 109}, {0, 114}, {0, 119}, {0, 124}, {0, 129}, 
                      {0, 134}, {0, 139}, {0, 144}, {0, 149}, {0, 154}, {0, 159}, {0, 164}, {0, 169}, 
                      {0, 174}, {0, 179}, {0, 184}};
        }

        else if((TruePegPosition > 13) && (TruePegPosition <= 20))
        {
            // 187 pixel jump
            Coords = {{0, 4}, {0, 9}, {0, 14}, {0, 19}, {0, 24}, {0, 29}, {0, 34}, {0, 39}, {0, 44}, 
                      {0, 49}, {0, 54}, {0, 59}, {0, 64}, {0, 69}, {0, 74}, {0, 79}, {0, 84}, {0, 89}, 
                      {0, 94}, {0, 99}, {0, 104}, {0, 109}, {0, 114}, {0, 119}, {0, 124}, {0, 129}, 
                      {0, 134}, {0, 139}, {0, 144}, {0, 149}, {0, 154}, {0, 159}, {0, 164}, {0, 169}, 
                      {0, 174}, {0, 179}, {0, 184}, {0, 187}};
        }

        else if((TruePegPosition > 20) && (TruePegPosition <= 27))
        {
            // 183 pixel jump
            Coords = {{0, 4}, {0, 9}, {0, 14}, {0, 19}, {0, 24}, {0, 29}, {0, 34}, {0, 39}, {0, 44}, 
                      {0, 49}, {0, 54}, {0, 59}, {0, 64}, {0, 69}, {0, 74}, {0, 79}, {0, 84}, {0, 89}, 
                      {0, 94}, {0, 99}, {0, 104}, {0, 109}, {0, 114}, {0, 119}, {0, 124}, {0, 129}, 
                      {0, 134}, {0, 139}, {0, 144}, {0, 149}, {0, 154}, {0, 159}, {0, 164}, {0, 169}, 
                      {0, 174}, {0, 179}, {0, 183}};
        }
    }


    // East
    if(Direction == 36)
    {
        Coords = {{4, -31}, {9, -47}, {14, -58}, {19, -67}, {24, -74}, {29, -81}, {34, -87}, {39, -92}, 
                  {44, -96}, {49, -100}, {54, -104}, {59, -108}, {64, -111}, {69, -113}, {74, -116}, {79, -118}, 
                  {84, -120}, {89, -122}, {94, -123}, {99, -125}, {104, -126}, {109, -127}, {114, -127}, {119, -128}, 
                  {124, -128}, {129, -128}, {134, -128}, {139, -128}, {144, -127}, {149, -126}, {154, -125}, {159, -124}, 
                  {164, -123}, {169, -121}, {174, -120}, {179, -118}, {184, -115}, {189, -113}, {194, -110}, {199, -107}, 
                  {204, -103}, {209, -100}, {214, -95}, {219, -91}, {224, -85}, {229, -80}, {234, -73}, {239, -65}, {244, -56}, 
                  {249, -44}, {254, -27}, {257, 0}};
    }

    // West
    if(Direction == 37)
    {
        Coords = {{-4, -31}, {-9, -47}, {-14, -58}, {-19, -67}, {-24, -74}, {-29, -81}, {-34, -87}, {-39, -92}, 
                  {-44, -96}, {-49, -100}, {-54, -104}, {-59, -108}, {-64, -111}, {-69, -113}, {-74, -116}, {-79, -118}, 
                  {-84, -120}, {-89, -122}, {-94, -123}, {-99, -125}, {-104, -126}, {-109, -127}, {-114, -127}, {-119, -128}, 
                  {-124, -128}, {-129, -128}, {-134, -128}, {-139, -128}, {-144, -127}, {-149, -126}, {-154, -125}, {-159, -124}, 
                  {-164, -123}, {-169, -121}, {-174, -120}, {-179, -118}, {-184, -115}, {-189, -113}, {-194, -110}, {-199, -107}, 
                  {-204, -103}, {-209, -100}, {-214, -95}, {-219, -91}, {-224, -85}, {-229, -80}, {-234, -73}, {-239, -65}, {-244, -56}, 
                  {-249, -44}, {-254, -27}, {-257, 0}};
    }
    
    // Iterate and add the new X and Y values of the current position of the texture.
    for(int XValue = 0; XValue < int(Coords.size());)
    {   
        vector<int> Vector;

        // Push the values into the the vector that will be returned
        Vector.push_back(DisplaySizeW / (1920.0 / Coords[XValue][0]) + PosX);
        Vector.push_back(DisplaySizeW / (1920.0 / Coords[XValue][1]) + PosY);

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
        Vector.push_back(0);
        Vector.push_back(-XValue);

        // All coords
        cout << "{" << Vector[0] << ", " << Vector[1] << "}, ";

        Vector.clear();

        XValue++;
    }

    cout << "};" << endl;
}

#endif