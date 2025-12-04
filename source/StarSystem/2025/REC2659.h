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
        return (x == other.x && y == other.y);
    }
    bool operator!=(float2 const& other) const 
    {
        return (x != other.x || y != other.y);
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

static bool pointInBox(float2 point, float2 topleft, float2 botright)
{
    return (topleft.x <= point.x && topleft.y >= point.y)
    && (point.x < botright.x && point.y > botright.y);
}

struct QuadNode
{
    
    float2 topleft, botright; // Bounding box of Node
    size_t max_capacity;
    bool isSubdivided = false;

    std::vector<float2> stars;
    QuadNode *tl_child = nullptr; // TOP LEFT CHILD
    QuadNode *bl_child = nullptr; // BOTTOM LEFT CHILD
    QuadNode *tr_child = nullptr; // TOP RIGHT CHILD
    QuadNode *br_child = nullptr; // BOTTOM RIGHT CHILD

    QuadNode(float2 topleft, float2 botright, size_t max_capacity): topleft(topleft), botright(botright), max_capacity(max_capacity) {}

    void insertStarRecurcively(float2 star);
    std::vector<float2> getStarsInBounds(float2 tl, float2 br);
private:
    void subdivide();
    bool isStarInBounds(float2 star);
    bool isColliding(float2 tl, float2 br);
};

static std::ostream& operator<<(std::ostream &os, QuadNode const &qn)                                                                                  //for struct output
{
    os << "Topleft: " << qn.topleft << " Botright: " << qn.botright << std::endl;
    if (qn.tl_child != nullptr)
    {
        os << "tl_child: " << *qn.tl_child;
        os << "bl_child: " << *qn.bl_child;
        os << "tr_child: " << *qn.tr_child;
        os << "br_child: " << *qn.br_child;
    }
    return os;
};

struct QuadTree
{
    QuadNode *root;

    QuadTree(float2 topleft, float2 botright, size_t max_capacity)
    {
        root = new QuadNode(topleft, botright, max_capacity);
    }
    void insertStar(float2 star);
    std::vector<float2> getNeighbours(float2 star, float radius);
};



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

    void addEdge(Edge e)
    {
        edges.push_back(e);
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
