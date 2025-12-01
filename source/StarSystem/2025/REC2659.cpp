#include "REC2659.h"
#include <Helpers.h>
#include <stdexcept>

namespace REC2659
{

void GlobalInit()
{
}

void GlobalTeardown()
{
}

float2 FindPointOnBisection(const float2 &p, const float2 &q, const float2 &mid)
{
    float slope_pq = (q.y - p.y) / (q.x - p.x);
    float slope_bisection = -1 / slope_pq;
    // std::cout << "slope of line: " << slope_pq << " slope bisection: " << slope_bisection << std::endl;
    // Line equation y = mx + b, where m is the slope and x&y a point on the line
    float b = mid.y - (slope_bisection * mid.x);
  
    return float2(
        mid.x + 1.0f,// Ensures I get a different point on the bisection
        slope_bisection * (mid.x + 1.0f) + b// The line formula for mid.x + 1
    );
}

float2 FindLineLineIntersection(float2 p1, float2 p2, float2 p3, float2 p4)
{
    float denominator = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
    if (denominator == 0)//! The line are parallel or coincident
        return float2();

    float pX = ((p1.x * p2.y - p1.y * p2.x) * (p3.x - p4.x) - (p1.x - p2.x) * (p3.x * p4.y - p3.y * p4.x)) / denominator;
    float pY = ((p1.x * p2.y - p1.y * p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x * p4.y - p3.y * p4.x)) / denominator;

    return float2(pX, pY);
}

Triangle equilateralTriangleFromIncircle(Circle cc)
{
    // Distance from center of cercle to vertexes of triangle
    double R = (2.0f * cc.radius) / std::sqrt(3.0f);

    // Fixed Angles : 0°, 120°, 240°
    Triangle t = Triangle();
    t.a = {(float)(cc.center.x + R * std::cos(0.0f)), (float)(cc.center.y + R * std::sin(0.0f))};
    t.b = {(float)(cc.center.x + R * std::cos(2.0f * M_PI / 3.0f)), (float)(cc.center.y + R * std::sin(2.0f * M_PI / 3.0f))};
    t.c = {(float)(cc.center.x + R * std::cos(4.0f * M_PI / 3.0f)), (float)(cc.center.y + R * std::sin(4.0f * M_PI / 3.0f))};

    return t;
}

Circle ComputeCircumCircle(const float2 &a, const float2 &b, const float2 &c)
{
    float2 bc_mid((b.x + c.x) / 2.0f, (b.y + c.y) / 2.0f);
    float2 ab_mid((a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f);


    float2 bc_bisec = FindPointOnBisection(b, c, bc_mid);
    float2 ab_bisec = FindPointOnBisection(a, b, ab_mid);

    // std::cout << "Line[bc] " << b << "|" << c << "-mid->" << bc_mid <<std::endl;
    // std::cout << "bc_bisec: " << bc_bisec << std::endl;
    // std::cout << "Line[ab] " << a << "|" << b << "-mid->" << ab_mid <<std::endl;
    // std::cout << "ab_bisec: " << ab_bisec << std::endl;


    float2 center_cc = FindLineLineIntersection(bc_mid, bc_bisec, ab_mid, ab_bisec);
    return Circle(center_cc, center_cc.distance(a));
}

std::vector<TrianglePos> BowyerWatsonDelaunayTriangulation(std::vector<float2> const &starSystem)
{
    int n = starSystem.size();
    std::vector<TrianglePos> triangles;

    // fill triangle with 0, 1, 2
    // Build SUPER triangle with the 2 most extreme points and their bisection

    if (n < 3) return triangles;

    float minX = starSystem[0].x;
    float maxX = starSystem[0].x;
    float minY = starSystem[0].y;
    float maxY = starSystem[0].y;

    for (float2 const &star : starSystem)
    {
        minX = std::min(minX, star.x);
        maxX = std::max(maxX, star.x);
        minY = std::min(minY, star.y);
        maxY = std::max(maxY, star.y);
    }

    float dx = maxX - minX; // The witdh of the solar system
    float dy = maxY - minY; // The hight of the solar system

    float diameter = std::max(dx, dy) * 2.0f;
    Triangle superT = equilateralTriangleFromIncircle(Circle({dx / 2.0f, dy / 2.0f}, diameter));

    std::cout << "SUPER TRIANGLE: " << superT << std::endl;

    std::vector<float2> working_points = starSystem;
    int i1 = working_points.size();
    working_points.push_back(superT.a);

    int i2 = working_points.size();
    working_points.push_back(superT.b);

    int i3 = working_points.size();
    working_points.push_back(superT.c); 

    triangles.push_back({i1, i2, i3});

    std::cout << starSystem.size() << std::endl;

    for (int i = 0; i < n; i++)
    {
        const float2 &star = working_points[i];
        std::cout << "current star: " << star << std::endl;

        std::vector<TrianglePos> badTriangles; // Triangles that violate Delaunay triangulation rule
        std::vector<Circle> circles;

        for (TrianglePos &t : triangles)
        {
            std::cout << "Triangle tested is made from{" << std::endl;
            std::cout << working_points[t.a] << std::endl;
            std::cout << working_points[t.b] << std::endl;
            std::cout << working_points[t.c] << std::endl << "}" << std::endl;
            
            Circle cc = ComputeCircumCircle(
                working_points[t.a],
                working_points[t.b],
                working_points[t.c]
            );
            std::cout << "Circumcircle of current triangle is: " << cc << std::endl;


            // distance between center of cc of current triangle and the star we are looking at
            std::cout << "Distance center cc and current star: " << star.distance(cc.center) << std::endl;
            if (star.distance(cc.center) < cc.radius)
            {
                badTriangles.push_back(t);
            }            
        }

        std::vector<Edge> edges;
        auto AddEdge = [&](Edge e)
        {
            for (std::vector<Edge>::iterator it = edges.begin(); it != edges.end(); it++)
            {
                if (*it == e) // Remove element if allready presentto avoid duplicates
                {
                    edges.erase(it);
                    return;
                }
            }
            edges.push_back(e);
        };

        for (TrianglePos bt : badTriangles)
        {
            AddEdge({bt.a, bt.b});
            AddEdge({bt.b, bt.c});
            AddEdge({bt.c, bt.a});
        }

        triangles.erase(
            std::remove_if(triangles.begin(), triangles.end(),
            [&](TrianglePos const &t)
            {
                for (TrianglePos bt : badTriangles)
                {
                    if (t.a == bt.a && t.b == bt.b && t.c == bt.c)
                        return true;
                }
                return false;
            }),
            triangles.end()
        );

        for (Edge &e : edges)
        {
            triangles.push_back({e.u, e.v, i});
        }
    }

    triangles.erase(
        std::remove_if(triangles.begin(), triangles.end(),
        [&](TrianglePos const &t)
        {
            return (t.a >= n || t.b >= n || t.c >= n);
        }),
        triangles.end()
    );

    return triangles;
}

float FindTheLowestDInTheStarSystem(std::vector<float2> const &starSystem)
{
    // UNUSED_PARAM(starSystem);
    // float2 a{-5, 4};
    // float2 b{4, 3};
    // float2 c{1, -2};
    // std::cout << "Test cc " << ComputeCircumCircle(b, c, a) << std::endl;

    std::vector<TrianglePos> delaunayTriangulation = BowyerWatsonDelaunayTriangulation(starSystem);
    std::cout << delaunayTriangulation.size() << std::endl;

    for (TrianglePos t : delaunayTriangulation)
    {
        std::cout << "First triangle has points{" << std::endl;
        std::cout << starSystem[t.a] << std::endl;
        std::cout << starSystem[t.b] << std::endl;
        std::cout << starSystem[t.c] << std::endl << "}";
    }
    return 0;
}

} // namespace REC2659
