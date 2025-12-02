#ifndef REC2659_H
#define REC2659_H

#include <string>
#include <vector>
#include <cmath>
#include <iostream>

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
    float distance(float2 other) const
    {
        return float(sqrt(pow(other.x - x, 2) + pow(other.y - y, 2))); 
    }
    bool operator==(float2 const& other) const 
    {
        return (x == other.x && y== other.y);
    }
};

static std::ostream& operator<<(std::ostream &os, float2 const &point)                                                                                  //for struct output
{
    os << "(" << point.x << ", " << point.y << ")";
    return os;
}

struct Edge
{
    float2* u;
    float2* v;
    Edge(float2* u, float2* v)
    {
        this->u = u;
        this->v = v;
    }
    bool operator==(Edge const& other) const 
    {
        return (u == other.u && v == other.v) || (u == other.v && v == other.u);
    }
    bool equal(const float2 &p, const float2 &q) const 
    {
        return (p == *u && q == *v) || (p == *v && q == *u);
    }
};

struct Triangle
{
    float2* vertices[3];
    Triangle* neighbours[3];
    bool containsEdge(const Edge &edge)
    {
        return edge.equal(*vertices[0], *vertices[1])
        || edge.equal(*vertices[1], *vertices[2]) 
        || edge.equal(*vertices[2], *vertices[0]);
    }
    bool contains(const float2 &point)
    {
        return *vertices[0] == point || *vertices[1] == point || *vertices[2] == point;
    }
};
static std::ostream& operator<<(std::ostream &os, Triangle const &t)                                                                                  //for struct output
{
    os << "Triangle {" << std::endl
    << t.vertices[0] << std::endl
    << t.vertices[1] << std::endl
    << t.vertices[2] << std::endl
    << "}" << std::endl;
    return os;
};

struct Circle
{
    float2 center;
    float radius;

    Circle(float2 center, float radius): center(center), radius(radius) {}
};

static std::ostream& operator<<(std::ostream &os, Circle const &cc)                                                                                  //for struct output
{
    os << "center:" << cc.center << " & radius:" << cc.radius;
    return os;
}

float2 FindPointOnBisection(const float2 &a, const float2 &b, const float2 &mid);
float2 FindLineLineIntersection(float2 p1, float2 p2, float2 p3, float2 p4);
Circle ComputeCircumCircle(Triangle *t);

Triangle* equilateralTriangleFromIncircle(Circle cc);


void GlobalInit();
void GlobalTeardown();
float FindTheLowestDInTheStarSystem(std::vector<float2> const& starSystem);

} // namespace REC2659

#endif // REC2659_H
