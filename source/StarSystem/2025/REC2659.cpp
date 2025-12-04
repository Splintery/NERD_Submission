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

/**
 * @brief
 * Inserts the star into the QuadNode, subdivinding and redistributing if capacity is reached
 * @param star the star to insert into the QuadTree
 */
void QuadNode::insertStarRecurcively(float2 star)
{
    if (isSubdivided) // Check if subdivided
    {
        if (tl_child->isStarInBounds(star)) tl_child->insertStarRecurcively(star);
        else if (bl_child->isStarInBounds(star)) bl_child->insertStarRecurcively(star);
        else if (tr_child->isStarInBounds(star)) tr_child->insertStarRecurcively(star);
        else if (br_child->isStarInBounds(star)) br_child->insertStarRecurcively(star);
    }
    else
    {
        // std::cout << "Inserting star:" << star << " my tl: " << topleft << " my br: " << botright << " isInBounds(): " << isStarInBounds(star) << std::endl;
        if (!isStarInBounds(star)) return; //! Will not happen but here for logic
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

/**
 * @brief
 * It is easier to express 2 rectangles not collinding and flipping the boolean than to check 
 * if a rectangle is inside/over another
 * @param tl topleft of rectangle we check for collision
 * @param br bottomright of rectangle we check for collision
 */
bool QuadNode::isColliding(float2 tl, float2 br)
{
    return !(br.x < topleft.x || tl.x > botright.x // if to the left or the right
    || br.y > topleft.y || tl.y < botright.y); // if above or under
}

/**
 * @brief
 * Retrieves every star contained in the rectangle described by "tl" and "br"
 * @param tl topleft of rectangle we search in
 * @param br bottomright of rectangle we search in
 */
std::vector<float2> QuadNode::getStarsInBounds(float2 tl, float2 br)
{
    std::vector<float2> res;
    if (isColliding(tl, br))
    {
        if (isSubdivided) // is subdivided
        {
            for (float2 star : tl_child->getStarsInBounds(tl, br))
                res.push_back(star);
            for (float2 star : bl_child->getStarsInBounds(tl, br))
                res.push_back(star);
            for (float2 star : tr_child->getStarsInBounds(tl, br))
                res.push_back(star);
            for (float2 star : br_child->getStarsInBounds(tl, br))
                res.push_back(star);
        }
        else
        {
            for (float2 star : stars)
            {
                if (pointInBox(star, tl, br))
                {
                    res.push_back(star);
                }
            }
        }
    }
    return res;
}

/**
 * @brief
 * Subdivides the space into 4 smaller rectangles, covering the same area and quadrupling the storage
 */
void QuadNode::subdivide()
{
    isSubdivided = true;
    float mid_x = (topleft.x + botright.x) / 2.0f;
    float mid_y = (topleft.y + botright.y) / 2.0f;

    tl_child = new QuadNode(topleft, {mid_x, mid_y}, max_capacity);
    bl_child = new QuadNode({topleft.x, mid_y}, {mid_x, botright.y}, max_capacity);
    tr_child = new QuadNode({mid_x, topleft.y}, {botright.x, mid_y}, max_capacity);
    br_child = new QuadNode({mid_x, mid_y}, botright, max_capacity);
}

bool QuadNode::isStarInBounds(float2 star) // Strictly in bounds
{
    return (topleft.x <= star.x && topleft.y >= star.y)
    && (star.x < botright.x && star.y > botright.y);
}


void QuadTree::insertStar(float2 star)
{
    root->insertStarRecurcively(star);
}

/**
 * @brief
 * Queries the QuadTree and returns any star within radius of "star"
 * 
 * @param star the star we wan't to find neighbours for
 * @param radius the radius we will look around our star for neighbours
 */
std::vector<float2> QuadTree::getNeighbours(float2 star, float radius) // Dumb version first
{
    // We want every star in the box surrounding "star"
    float2 tl = {star.x - radius, star.y + radius};
    float2 br = {star.x + radius, star.y - radius};
    return root->getStarsInBounds(tl, br);
}

/**
 * @brief
 * Kruskal MST algorithm nothing new from: https://fr.wikipedia.org/wiki/Algorithme_de_Kruskal
 * Based on Union find structure: https://fr.wikipedia.org/wiki/Union-find
 */
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
/**
 * @brief
 * Finds the lowest D that makes the star system explorable
 * @param starsystem The star system given to work with
 */
float FindTheLowestDInTheStarSystem(std::vector<float2> const &starSystem)
{
    float max_x, max_y, min_x, min_y;
    max_x = starSystem.at(0).x;
    max_y = starSystem.at(0).y;
    min_x = starSystem.at(0).x;
    min_y = starSystem.at(0).y;

    for (float2 star : starSystem) // Finding limits of star system
    {
        max_x = std::max(max_x, star.x);
        max_y = std::max(max_y, star.y);
        min_x = std::min(min_x, star.x);
        min_y = std::min(min_y, star.y);
    }

    QuadTree qt = QuadTree({min_x - 1.0f, max_y + 1.0f}, {max_x + 1.0f, min_y - 1.0f}, 8); // I add a padding to avoid missing stars on edges
    for (float2 star : starSystem)
    {
        qt.insertStar(star);
    }

    // std::cout << "Done inserting in QuadTree" << *qt.root << std::endl;

    Graph reduced_graph = Graph();
    reduced_graph.setV(starSystem.size());

    int E = 0;
    std::vector<float2> neighbours;
    for (float2 star : starSystem)
    {
        float radius = 0.0025f;
        neighbours.clear();
        while (neighbours.size() < 4) // at least 3 of the closest neighbours
        {
            neighbours = qt.getNeighbours(star, radius);
            radius = radius * 2.0f; // We expand the radius until we are connected enough
            // std::cout << "doubling radius !" << std::endl;
        }

        // std::cout << "Got this many neighbours: " << neighbours.size() << std::endl;
        E+=neighbours.size() - 1;
        for (float2 n : neighbours) // We create the edges for each neighbour
        {
            if (n != star)
            {
                reduced_graph.addEdge({n, star});
            }
        }
        
    }
    reduced_graph.setE(E);

    // std::cout << "Running Kruskal" << std::endl;
    return reduced_graph.kruskalMST();
}

} // namespace REC2659
