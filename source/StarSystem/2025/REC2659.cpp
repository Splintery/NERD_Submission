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
    t.vertices[0] = float2((float)(cc.center.x + R * std::cos(0.0f)), (float)(cc.center.y + R * std::sin(0.0f)));
    t.vertices[1] = float2((float)(cc.center.x + R * std::cos(2.0f * M_PI / 3.0f)), (float)(cc.center.y + R * std::sin(2.0f * M_PI / 3.0f)));
    t.vertices[2] = float2((float)(cc.center.x + R * std::cos(4.0f * M_PI / 3.0f)), (float)(cc.center.y + R * std::sin(4.0f * M_PI / 3.0f)));

    return t;
}

Circle ComputeCircumCircle(Triangle t)
{
    float2 bc_mid((t.vertices[1].x + t.vertices[2].x) / 2.0f, (t.vertices[1].y + t.vertices[2].y) / 2.0f);
    float2 ab_mid((t.vertices[0].x + t.vertices[1].x) / 2.0f, (t.vertices[0].y + t.vertices[1].y) / 2.0f);


    float2 bc_bisec = FindPointOnBisection(t.vertices[1], t.vertices[2], bc_mid);
    float2 ab_bisec = FindPointOnBisection(t.vertices[0], t.vertices[1], ab_mid);

    // std::cout << "Line[bc] " << b << "|" << c << "-mid->" << bc_mid <<std::endl;
    // std::cout << "bc_bisec: " << bc_bisec << std::endl;
    // std::cout << "Line[ab] " << t->vertices[0] << "|" << b << "-mid->" << ab_mid <<std::endl;
    // std::cout << "ab_bisec: " << ab_bisec << std::endl;


    float2 center_cc = FindLineLineIntersection(bc_mid, bc_bisec, ab_mid, ab_bisec);
    return Circle(center_cc, center_cc.distance(t.vertices[0]));
}

Triangle* WalkToContainingTriangle(std::vector<Triangle*> &triangulation, float2 p)
{
    return nullptr;
}

std::vector<Triangle> BowyerWatson(std::vector<float2> const &starSystem)
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
    Triangle superT = equilateralTriangleFromIncircle(Circle({dx / 2.0f, dy / 2.0f}, diameter));
    std::cout << "Super Triangle is: " << superT << std::endl;

    std::vector<Triangle> triangulation = {superT};

    // Step 2 insert the point 1 by 1
    for (float2 p : starSystem)
    {
        std::cout << "working on point: " << p << std::endl;
        std::vector<Triangle> badTriangles;
        for (Triangle t : triangulation)
        {
            Circle cc = ComputeCircumCircle(t);
            if (p.distance(cc.center) < cc.radius)
            {
                std::cout << "added this triangle to bad triangles: " << t << std::endl;
                badTriangles.push_back(t);
            }
        }

        std::vector<Edge> polygon;
        /*
        for each triangle in badTriangles do // find the boundary of the polygonal hole
            for each edge in triangle do
                if edge is not shared by any other triangles in badTriangles
                    add edge to polygon
        */
        for (size_t i = 0; i < badTriangles.size(); i++)
        {
            // Triangle ABC with A = 0, B = 1 & C = 2 in vertices[]
            Edge ab = Edge(badTriangles.at(i).vertices[0], badTriangles.at(i).vertices[1]);
            Edge bc = Edge(badTriangles.at(i).vertices[1], badTriangles.at(i).vertices[2]);
            Edge ca = Edge(badTriangles.at(i).vertices[2], badTriangles.at(i).vertices[0]);
            bool ab_shared = false;
            bool bc_shared = false;
            bool ca_shared = false;

            std::cout << "Bad triangle made of the follozing edges: " << ab << " & " << bc << " & " << ca << std::endl;
            printf("Booleans state: ab(%b), bc(%b), ca(%b)\n", ab_shared, bc_shared, ca_shared);

            for (size_t j = 0; j < badTriangles.size(); j++)
            {
                std::cout << "test5" << std::endl;

                if (i != j) 
                {
                    // Triangle DEF with D = 0, E = 1 & F = 2 in vertices[]
                    Edge de(badTriangles.at(j).vertices[0], badTriangles.at(j).vertices[1]);
                    Edge ef(badTriangles.at(j).vertices[1], badTriangles.at(j).vertices[2]);
                    Edge fd(badTriangles.at(j).vertices[2], badTriangles.at(j).vertices[0]);
                    std::cout << "test3" << std::endl;

                    if (ab == de || ab == ef || ab == fd) ab_shared = true;
                    if (bc == de || bc == ef || bc == fd) bc_shared = true;
                    if (ca == de || ca == ef || ca == fd) ca_shared = true;
                }
                std::cout << "test6" << std::endl;
                
                std::cout << "test4" << std::endl;

            }
            printf("Booleans state: ab(%b), bc(%b), ca(%b)\n", ab_shared, bc_shared, ca_shared);

            if (!ab_shared) polygon.push_back(ab);
            if (!bc_shared) polygon.push_back(bc);
            if (!ca_shared) polygon.push_back(ca);
        }
        std::cout << "test7" << std::endl;

        for (std::vector<Triangle>::iterator it = triangulation.begin(); it != triangulation.end();)
        {
            if (std::find(badTriangles.begin(), badTriangles.end(), *it) != badTriangles.end())
            {
                it = triangulation.erase(it);
            }
            else
            {
                it++;
            }
        }

        std::cout << "test8" << std::endl;

        // re-triangulate the polygonal hole
        for (Edge e : polygon)
        {
            Triangle newTri = Triangle();
            newTri.vertices[0] = p;
            newTri.vertices[1] = e.u;
            newTri.vertices[2] = e.v;
            triangulation.push_back(newTri);
        }   
        std::cout << "test9" << std::endl;
    }
    /*
    for each triangle in triangulation // done inserting points, now clean up
        if triangle contains a vertex from original super-triangle
            remove triangle from triangulation
    */
    for (std::vector<Triangle>::iterator it = triangulation.begin(); it != triangulation.end();)
    {
        if (it->contains(superT.vertices[0]) || it->contains(superT.vertices[1]) || it->contains(superT.vertices[2]))
        {
            it = triangulation.erase(it);
        }
        else
        {
            it++;
        }
    }
    return triangulation;
}

float Graph::kruskalMST()
{
    float longuest_edge = 0;
    std::sort(this->edges.begin(), this->edges.end());

    DisjointSets ds(this->V);

    for (std::vector<Edge>::iterator it = this->edges.begin(); it != this->edges.end(); it++)
    {

        int id_u = getId(it->u);
        int id_v = getId(it->v);

        int set_u = ds.find(id_u);
        int set_v = ds.find(id_v);

        if (set_u != set_v)
        {
            longuest_edge = std::max(longuest_edge, it->length());
            std::cout << *it << " length: " << it->length() << std::endl;
            ds.merge(set_u, set_v);
        }
    }
    return longuest_edge;
}

float FindTheLowestDInTheStarSystem(std::vector<float2> const &starSystem)
{
    // UNUSED_PARAM(starSystem);
    // float2 a{-5, 4};
    // float2 b{4, 3};
    // float2 c{1, -2};
    // std::cout << "Test cc " << ComputeCircumCircle(b, c, a) << std::endl;

    std::cout << "test1" << std::endl;
    std::vector<Triangle> dt = BowyerWatson(starSystem);
    std::cout << "test2" << std::endl;
    

    Graph reduced_graph = Graph();
    reduced_graph.setV(starSystem.size());

    int E = 0;
    for (Triangle t : dt)
    {
        reduced_graph.addEdge(t);
        std::cout << t << std::endl;
        E++;
    }
    reduced_graph.setE(E);

    return reduced_graph.kruskalMST();
}

} // namespace REC2659
