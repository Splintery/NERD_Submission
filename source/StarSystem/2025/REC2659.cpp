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

Triangle* equilateralTriangleFromIncircle(Circle cc)
{
    // Distance from center of cercle to vertexes of triangle
    double R = (2.0f * cc.radius) / std::sqrt(3.0f);

    // Fixed Angles : 0°, 120°, 240°
    Triangle *t = new Triangle();
    t->vertices[0] = new float2((float)(cc.center.x + R * std::cos(0.0f)), (float)(cc.center.y + R * std::sin(0.0f)));
    t->vertices[1] = new float2((float)(cc.center.x + R * std::cos(2.0f * M_PI / 3.0f)), (float)(cc.center.y + R * std::sin(2.0f * M_PI / 3.0f)));
    t->vertices[2] = new float2((float)(cc.center.x + R * std::cos(4.0f * M_PI / 3.0f)), (float)(cc.center.y + R * std::sin(4.0f * M_PI / 3.0f)));

    return t;
}

Circle ComputeCircumCircle(Triangle* t)
{
    float2 bc_mid((t->vertices[1]->x + t->vertices[2]->x) / 2.0f, (t->vertices[1]->y + t->vertices[2]->y) / 2.0f);
    float2 ab_mid((t->vertices[0]->x + t->vertices[1]->x) / 2.0f, (t->vertices[0]->y + t->vertices[1]->y) / 2.0f);


    float2 bc_bisec = FindPointOnBisection(*t->vertices[1], *t->vertices[2], bc_mid);
    float2 ab_bisec = FindPointOnBisection(*t->vertices[0], *t->vertices[1], ab_mid);

    // std::cout << "Line[bc] " << b << "|" << c << "-mid->" << bc_mid <<std::endl;
    // std::cout << "bc_bisec: " << bc_bisec << std::endl;
    // std::cout << "Line[ab] " << t->vertices[0] << "|" << b << "-mid->" << ab_mid <<std::endl;
    // std::cout << "ab_bisec: " << ab_bisec << std::endl;


    float2 center_cc = FindLineLineIntersection(bc_mid, bc_bisec, ab_mid, ab_bisec);
    return Circle(center_cc, center_cc.distance(*t->vertices[0]));
}

Triangle* WalkToContainingTriangle(std::vector<Triangle*> &triangulation, float2 p)
{
    return nullptr;
}

std::vector<Triangle*> BowyerWatson(std::vector<float2> const &starSystem)
{
    // Init super triangle
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
    Triangle *superT = equilateralTriangleFromIncircle(Circle({dx / 2.0f, dy / 2.0f}, diameter));

    superT->neighbours[0] = superT->neighbours[1] = superT->neighbours[2] = nullptr;
    std::vector<Triangle*> triangulation = {superT};

    // Step 2 insert the point 1 by 1
    for (float2 p : starSystem)
    {
        std::vector<Triangle*> badTriangles;
        for (Triangle *t : triangulation)
        {
            Circle cc = ComputeCircumCircle(t);
            if (p.distance(cc.center) < cc.radius)
            {
                badTriangles.push_back(t);
            }
        }

        std::vector<Edge*> polygon;
        /*
        for each triangle in badTriangles do // find the boundary of the polygonal hole
            for each edge in triangle do
                if edge is not shared by any other triangles in badTriangles
                    add edge to polygon
        */
        for (size_t i = 0; i < badTriangles.size(); i++)
        {
            // Triangle ABC with A = 0, B = 1 & C = 2 in vertices[]
            Edge *ab = new Edge(badTriangles.at(i)->vertices[0], badTriangles.at(i)->vertices[1]);
            Edge *bc = new Edge(badTriangles.at(i)->vertices[1], badTriangles.at(i)->vertices[2]);
            Edge *ca = new Edge(badTriangles.at(i)->vertices[2], badTriangles.at(i)->vertices[0]);
            bool ab_shared, bc_shared, ca_shared = false;

            for (size_t j = 0; j < badTriangles.size(); j++)
            {
                if (i == j) continue;
                // Triangle DEF with D = 0, E = 1 & F = 2 in vertices[]
                Edge de(badTriangles.at(j)->vertices[0], badTriangles.at(j)->vertices[1]);
                Edge ef(badTriangles.at(j)->vertices[1], badTriangles.at(j)->vertices[2]);
                Edge fd(badTriangles.at(j)->vertices[2], badTriangles.at(j)->vertices[0]);

                if (*ab == de || *ab == ef || *ab == fd) ab_shared = true;
                if (*bc == de || *bc == ef || *bc == fd) bc_shared = true;
                if (*ca == de || *ca == ef || *ca == fd) ca_shared = true;
            }

            if (!ab_shared) polygon.push_back(ab);
            else delete ab;

            if (!bc_shared) polygon.push_back(bc);
            else delete bc;

            if (!ca_shared) polygon.push_back(ca);
            else delete ca;
        }

        for (std::vector<Triangle*>::iterator it = triangulation.begin(); it != triangulation.end(); it++)
        {
            for (Triangle *bt : badTriangles)
            {
                if (*it == bt) triangulation.erase(it);
            }
        }

        // re-triangulate the polygonal hole
        for (Edge *e : polygon)
        {
            Triangle *newTri = new Triangle();
            newTri->vertices[0] = new float2(p);
            newTri->vertices[1] = e->u;
            newTri->vertices[2] = e->v;
            triangulation.push_back(newTri);
        }   
    }
    /*
    for each triangle in triangulation // done inserting points, now clean up
        if triangle contains a vertex from original super-triangle
            remove triangle from triangulation
    */
    for (std::vector<Triangle*>::iterator it = triangulation.begin(); it != triangulation.end(); it++)
    {
        if ((*it)->contains(*superT->vertices[0]) || (*it)->contains(*superT->vertices[0]) || (*it)->contains(*superT->vertices[0]))
        {
            triangulation.erase(it);
        }
    }
    return triangulation;
}

float FindTheLowestDInTheStarSystem(std::vector<float2> const &starSystem)
{
    // UNUSED_PARAM(starSystem);
    // float2 a{-5, 4};
    // float2 b{4, 3};
    // float2 c{1, -2};
    // std::cout << "Test cc " << ComputeCircumCircle(b, c, a) << std::endl;

    // std::vector<TrianglePos> delaunayTriangulation = BowyerWatsonDelaunayTriangulation(starSystem);
    // std::cout << delaunayTriangulation.size() << std::endl;

    // for (TrianglePos t : delaunayTriangulation)
    // {
    //     std::cout << "First triangle has points{" << std::endl;
    //     std::cout << starSystem[t.a] << std::endl;
    //     std::cout << starSystem[t.b] << std::endl;
    //     std::cout << starSystem[t.c] << std::endl << "}";
    // }
    return 0;
}

} // namespace REC2659
