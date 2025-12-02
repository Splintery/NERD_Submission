## Train of thoughts
* The given star sytem can be seen as a complete graph that could potentially be composed of millions of vertexes.
* To find the lowest distance that makes the solar system explorable I would need to tranform this complete graph into a minimally connected graph (removing any edge would result into a disconnected graph) wich weight is the smallest possible (if the weight of an edge is its length).
* After pondering my graph knowledge for a moment I realised that what I was looking for is a MST (Minimum Spanning Tree).
* The smallest distance would then be the longest edge in the MST

## Non trivial subproblem
* Wandering wich algorithm I could use to compute the MST (https://en.wikipedia.org/wiki/Minimum_spanning_tree) I realised that the operation complexicty of these algorithm are expressed in terms of vertexes AND edges making it impossible as it is to be under O(n)
