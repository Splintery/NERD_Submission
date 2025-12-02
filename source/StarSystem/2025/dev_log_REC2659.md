# Train of thoughts
* The given star sytem can be seen as a complete graph that could potentially be composed of millions of vertexes.
* To find the lowest distance that makes the solar system explorable I would need to tranform this complete graph into a minimally connected graph (removing any edge would result into a disconnected graph) wich weight is the smallest possible (if the weight of an edge is its length).
* After pondering my graph knowledge for a moment I realised that what I was looking for is a MST (Minimum Spanning Tree).
* The smallest distance would then be the longest edge in the MST

## Non trivial subproblem
Wandering wich algorithm I could use to compute the MST (https://en.wikipedia.org/wiki/Minimum_spanning_tree) I realised that the operation complexicty of these algorithms are expressed in terms of vertexes AND edges making it impossible as it is to be under O(**n**²). With a complete graph of **n** vertexes and **m** the number of edges to lookup, **m** is equal to (**n** - 1) * **n**, so more or less allready O(**n**²).

# My solutions (good and bad)

I need to prune the edge of the graph. Find a way to simplify it.
## Bad solution n°1
* I first thaught of turning the 2D space into a 1D problem.
* I would have a tree like structure holding circles centered on the middle of the solar system.
Whenever a new star is inserted into the tree it would create a circle with radius == distance between point and middle of solarsystem.
* The Minimum D would be found between the largest difference of radius between 2 consecutive circles.
I didn't stick with this solution because I can't assume that there are enough stars on each circle to use this method. They could be very poorly distributed.

## Good solution n°2
* In my second year of Bachelors degree I studied Quadtree to optimise the GameOfLife
