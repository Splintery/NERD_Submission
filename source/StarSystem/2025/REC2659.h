#ifndef REC2659_H
#define REC2659_H

#include <string>
#include <vector>

namespace REC2659
{

struct int2
{
    int x;
    int y;

    int2(int X, int Y)
        : x(X)
        , y(Y)
    {}
    int2()
        : x(0)
        , y(0)
    {}
};
struct float2
{
    float x;
    float y;

    float2(float X, float Y)
        : x(X)
        , y(Y)
    {}
    float2()
        : x(0)
        , y(0)
    {}
    float2(int2 a)
        : x(float(a.x))
        , y(float(a.y))
    {}
};

void GlobalInit();
void GlobalTeardown();
float FindTheLowestDInTheStarSystem(std::vector<float2> const& starSystem);

} // namespace REC2659

#endif // REC2659_H
