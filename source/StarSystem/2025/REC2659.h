#ifndef REC2659_H
#define REC2659_H

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <tuple>
#include <unordered_map>

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

struct float2_hash {
    std::size_t operator()(float2 const& p) const noexcept {
        std::size_t h1 = std::hash<float>()(p.x);
        std::size_t h2 = std::hash<float>()(p.y);
        return h1 ^ (h2 << 1);
    }
};

struct Edge
{
    float2 u;
    float2 v;
    Edge(float2 u, float2 v): u(u), v(v) {}

    float length() const
    {
        return u.distance(v);
    }
    bool operator==(Edge const& other) const 
    {
        return (u == other.u && v == other.v) || (u == other.v && v == other.u);
    }
    bool operator<(Edge const& other) const 
    {
        return this->length() < other.length();
    }
    bool equal(const float2 &p, const float2 &q) const 
    {
        return (p == u && q == v) || (p == v && q == u);
    }
    
};
static std::ostream& operator<<(std::ostream &os, Edge const &e)                                                                                  //for struct output
{
    os << "Edge from: " << e.u << " to: " << e.v;
    return os;
};

//! Structure used in BowyerWatson's algorithm

struct Triangle
{
    float2 vertices[3];
    // Triangle* neighbours[3];
    bool containsEdge(const Edge &edge)
    {
        return edge.equal(vertices[0], vertices[1])
        || edge.equal(vertices[1], vertices[2]) 
        || edge.equal(vertices[2], vertices[0]);
    }
    bool contains(const float2 &point)
    {
        return vertices[0] == point || vertices[1] == point || vertices[2] == point;
    }
    std::tuple<Edge, Edge, Edge> getEdges()
    {
        return std::make_tuple(
            Edge(vertices[0], vertices[1]),
            Edge(vertices[1], vertices[2]),
            Edge(vertices[2], vertices[0]));
    }
    bool operator==(Triangle const& other) const 
    {
        return (vertices[0] == other.vertices[0] || vertices[0] == other.vertices[1] || vertices[0] == other.vertices[2])
            && (vertices[1] == other.vertices[0] || vertices[1] == other.vertices[1] || vertices[1] == other.vertices[2])
            && (vertices[2] == other.vertices[0] || vertices[2] == other.vertices[1] || vertices[2] == other.vertices[2]);
    }
};
static std::ostream& operator<<(std::ostream &os, Triangle const &t)                                                                                  //for struct output
{
    os << "Triangle {" << std::endl
    << t.vertices[0] << std::endl
    << t.vertices[1] << std::endl
    << t.vertices[2] << std::endl
    << "}";
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

//! Structure used in BowyerWatson's algorithm

//* Helper function for Bowyer Watson
float2 FindPointOnBisection(const float2 &a, const float2 &b, const float2 &mid);
float2 FindLineLineIntersection(float2 p1, float2 p2, float2 p3, float2 p4);
Circle ComputeCircumCircle(Triangle t);

Triangle equilateralTriangleFromIncircle(Circle cc);
std::vector<Triangle> BowyerWatson(std::vector<float2> const &starSystem);
//* Helper function for Bowyer Watson

//! Structure used in Kruskal algorithm
struct Graph
{
    int V, E;
    std::vector<Edge> edges;
    std::unordered_map<float2, int, float2_hash> id;
    int next_id = 0;

    Graph(int V, int E): V(V), E(E) {}
    Graph() {}
    void setV(int V)
    {
        this->V = V;
    }
    void setE(int E)
    {
        this->E = E;
    }

    void addEdge(Triangle t)
    {
        std::tuple<Edge, Edge, Edge> new_edges = t.getEdges();
        edges.push_back(std::get<0>(new_edges));
        edges.push_back(std::get<1>(new_edges));
        edges.push_back(std::get<2>(new_edges));
    }

    int getId(const float2& p) {
        auto it = id.find(p);
        if (it != id.end())
        {
            return it->second;
        }
        else
        {
            id[p] = next_id; // Tack this point in map
            return next_id++;
        }
    }

    float kruskalMST();
};

struct DisjointSets
{
    int *parent;
    int *rnk;
    int n;

    // Constructor.
    DisjointSets(int n)
    {
        // Allocate memory
        this->n = n;
        parent = new int[n+1];
        rnk = new int[n+1];

        // Initially, all vertices are in
        // different sets and have rank 0.
        for (int i = 0; i <= n; i++)
        {
            rnk[i] = 0;

            //every element is parent of itself
            parent[i] = i;
        }
    }

    // Find the parent of a node 'u'
    // Path Compression
    int find(int u)
    {
        /* Make the parent of the nodes in the path
        from u--> parent[u] point to parent[u] */
        if (u != parent[u])
            parent[u] = find(parent[u]);
        return parent[u];
    }

    // Union by rank
    void merge(int x, int y)
    {
        x = find(x), y = find(y);

        /* Make tree with smaller height
        a subtree of the other tree */
        if (rnk[x] > rnk[y])
            parent[y] = x;
        else // If rnk[x] <= rnk[y]
            parent[x] = y;

        if (rnk[x] == rnk[y])
            rnk[y]++;
    } 
};
//! Structure used in Kruskal algorithm

void GlobalInit();
void GlobalTeardown();
float FindTheLowestDInTheStarSystem(std::vector<float2> const& starSystem);

} // namespace REC2659

#endif // REC2659_H
