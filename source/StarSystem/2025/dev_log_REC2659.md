# Train of thoughts
* The given star sytem can be seen as a complete graph that could potentially be composed of millions of vertexes.
* To find the lowest distance that makes the solar system explorable I would need to tranform this complete graph into a minimally connected graph (removing any edge would result into a disconnected graph) wich weight is the smallest possible (if the weight of an edge is its length).
* After pondering my graph knowledge for a moment I realised that what I was looking for is a MST (Minimum Spanning Tree).
* The smallest distance would then be the longest edge in the MST

## Non trivial subproblem
Wandering wich algorithm I could use to compute the [MST](https://en.wikipedia.org/wiki/Minimum_spanning_tree) I realised that the operation complexicty of these algorithms are expressed in terms of vertexes AND edges making it impossible as it is to be under O(**n**²). With a complete graph of **n** vertexes and **m** the number of edges to lookup, **m** is equal to (**n** - 1) * **n**, so more or less allready O(**n**²).

# My solutions (good and bad)

I need to prune the edge of the graph. Find a way to simplify it.
## Bad solution n°1
* I first thaught of flattening the 2D space into a 1D problem.
* I would have a tree like structure holding circles centered on the middle of the solar system.
Whenever a new star is inserted into the tree it would create a circle with radius == distance between point and middle of solarsystem.
* The Minimum D would be found between the largest difference of radius between 2 consecutive circles.
I didn't stick with this solution because I can't assume that there are enough stars on each circle to use this method. They could be very poorly distributed.

## Ok solution n°2
* I got another idea based on the strongest structure known, the triangle. Last year when I worked on implementing a scheduler to coordinate multiple threads, I discorvered [Gueorgui Voronoï](https://fr.wikipedia.org/wiki/Gueorgui_Vorono%C3%AF) and his [diagram](https://fr.wikipedia.org/wiki/Diagramme_de_Vorono%C3%AF). The interresting fact I remembered was the link between a Voronoï diagram and a Delaunay [Triangulation](https://fr.wikipedia.org/wiki/Triangulation_de_Delaunay).
* To build a Delaunay Triangulation incrementaly whener testing a triangle I need to keep only the edges that build triangles whom circumcircle doesn't contain any other star. (Computing a delaunay triangulation in O(n*log(n)) is tedious and maybe overkill for the context)
* The result is a connected graph with 3n edges at most thanks to the triangle arrangement

## Good solution n°3
* In my second year of Bachelors degree I studied Quadtrees to optimise the GameOfLife
* Building a quad tree can be done efficiently if the stars aren't all aligned in a line (would result in a tree of height n)
* The quad tree could then be used to query the "best" neighbours of each star
* Harder to prove that the graph stays connected, but if I make sure that for every star that I query, I get at least 3 other neighbours that are closest, it is garranted to stay connected.

# Conclusion

After Implementing BowyerWatson's algorithm I had hit a bottleneck. This way of computing the triangulation requires to find every bad triangle for each point getting O(n²) if we simple iterate over the triangles allready formed. To use delaunay's triangulation I would have had to implement either the divide and conquer(by [John Guibas](https://fr.wikipedia.org/wiki/Leonidas_John_Guibas) and [Jorge Stolfi](https://fr.wikipedia.org/wiki/Jorge_Stolfi)) or the [Fortune sweep](https://en.wikipedia.org/wiki/Fortune%27s_algorithm) approach.

I Opted to submit my Quadtree approach as it yielded the fastest and most coherent result. My QuadTree is able to redistribute the load over its childrens as to avoid the line problem.

