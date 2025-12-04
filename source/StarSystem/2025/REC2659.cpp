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

void QuadNode::insertStarRecurcively(float2 star)
{
    if (tl_child != nullptr) // Check if subdivided
    {
        if (tl_child->isStarInBounds(star)) tl_child->insertStarRecurcively(star);
        else if (bl_child->isStarInBounds(star)) bl_child->insertStarRecurcively(star);
        else if (tr_child->isStarInBounds(star)) tr_child->insertStarRecurcively(star);
        else if (br_child->isStarInBounds(star)) br_child->insertStarRecurcively(star);
    }
    else
    {
        std::cout << "Inserting star:" << star << " my tl: " << topleft << " my br: " << botright << " isInBounds(): " << isStarInBounds(star) << std::endl;
        if (!isStarInBounds(star)) return;
        stars.push_back(star);

        if (stars.size() > max_capacity)
        {
            subdivide();
            for (float2 star : stars)
            {
                insertStarRecurcively(star);
            }
            stars.clear();
        }
    }
}
std::vector<float2> QuadNode::getNeighboursRec(float2 star)
{
    if (isStarInBounds(star))
    {
        if (tl_child != nullptr) // is subdivided
        {
            if (tl_child->isStarInBounds(star)) return tl_child->getNeighboursRec(star);
            else if (bl_child->isStarInBounds(star)) return bl_child->getNeighboursRec(star);
            else if (tr_child->isStarInBounds(star)) return tr_child->getNeighboursRec(star);
            else if (br_child->isStarInBounds(star)) return br_child->getNeighboursRec(star);
        }
        else
        {
            return stars;
        }
    }
    else
    {
        return {};
    }
}
void QuadNode::subdivide()
{
    float mid_x = (topleft.x + botright.x) / 2.0f;
    float mid_y = (topleft.y + botright.y) / 2.0f;

    tl_child = new QuadNode(topleft, {mid_x, mid_y}, max_capacity);
    bl_child = new QuadNode({topleft.x, mid_y}, {mid_x, botright.y}, max_capacity);
    tr_child = new QuadNode({mid_x, topleft.y}, {botright.x, mid_y}, max_capacity);
    br_child = new QuadNode({mid_x, mid_y}, botright, max_capacity);
}
bool QuadNode::isStarInBounds(float2 star) // Striclty in bounds
{
    return (topleft.x <= star.x && topleft.y >= star.y)
    && (star.x < botright.x && star.y > botright.y);
}


void QuadTree::insertStar(float2 star)
{
    root->insertStarRecurcively(star);
}
std::vector<float2> QuadTree::getNeighbours(float2 star) // Dumb version first
{
    return root->getNeighboursRec(star);
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
            ds.merge(set_u, set_v);
        }
    }
    return longuest_edge;
}

float FindTheLowestDInTheStarSystem(std::vector<float2> const &starSystem)
{
    float max_x, max_y, min_x, min_y;
    max_x = starSystem.at(0).x;
    max_y = starSystem.at(0).y;
    min_x = starSystem.at(0).x;
    min_y = starSystem.at(0).y;

    for (float2 star : starSystem)
    {
        max_x = std::max(max_x, star.x);
        max_y = std::max(max_y, star.y);
        min_x = std::min(min_x, star.x);
        min_y = std::min(min_y, star.y);
    }
    QuadTree qt = QuadTree({min_x, max_y}, {max_x, min_y}, 8);
    for (float2 star : starSystem)
    {
        qt.insertStar(star);
    }

    std::cout << "Done inserting in QuadTree" << *qt.root << std::endl;
    Graph reduced_graph = Graph();
    reduced_graph.setV(starSystem.size());

    int E = 0;
    for (float2 star : starSystem)
    {
        std::vector<float2> neighbours = qt.getNeighbours(star);
        std::cout << "Got this many neighbours: " << neighbours.size() << std::endl;
        E+=neighbours.size() - 1;
        if (neighbours.size() > 1) // will be avoided
        {
            for (float2 n : neighbours)
            {
                if (n != star)
                {
                    reduced_graph.addEdge({n, star});
                }
            }
        }
    }
    reduced_graph.setE(E);

    std::cout << "Running Kruskal" << std::endl;

    return reduced_graph.kruskalMST();
}

} // namespace REC2659
