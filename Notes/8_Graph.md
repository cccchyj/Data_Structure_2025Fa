# 图

## **21 Minimum Spanning Trees**（8.4 最小生成树）

To model the wiring problem, use a connected, undirected graph $G = (V, E)$, where $V$ is the set of pins, $E$ is the set of possible interconnections between pairs of pins, and for each edge $(u, v) \in E$, a weight $w(u, v)$ specifies the cost (amount of wire needed) to connect $u$ and $v$. The goal is to find an acyclic subset $T \subseteq E$ that connects all of the vertices and whose total weight $$w(T) = \sum_{(u,v) \in T} w(u, v)$$is minimized. Since $T$ is acyclic and connects all of the vertices, it must form a tree, which we call a **spanning tree** since it "spans" the graph $G$. We call the problem of determining the tree $T$ the **minimum-spanning-tree problem**.

This chapter studies two ways to solve the minimum-spanning-tree problem. Kruskal's algorithm and Prim's algorithm both run in $O(E lg V)$ time.

### **21.1    Growing a minimum spanning tree**

The generic method manages a set $A$ of edges, maintaining the following loop invariant: Prior to each iteration, _A_ is a subset of some minimum spanning tree.

|     | GENERIC-MST(_G_, _w_)                              |
| --- | -------------------------------------------------- |
| 1   | _A_ = Ø                                            |
| 2   | **while** A does not form a spanning tree          |
| 3   | `  `find an edge (_u_, _v_) that is safe for _A_ |
| 4   | `  `_A_ = _A_ ∪ {(_u_, _v_)}                     |
| 5   | **return** _A_                                     |

Each step determines an edge $(u, v)$ that the procedure can add to $A$ without violating this invariant, in the sense that $A ∪ {(u, v)}$ is also a subset of a minimum spanning tree. We call such an edge a ***safe edge*** for $A$, since it can be added safely to $A$ while maintaining the invariant.

We first need some definitions. A **_cut_** (S, _V_ – S) of an undirected graph _G_ = (_V_, _E_) is a partition of _V_. We say that an edge (_u_, _v_) ∈ _E_**_crosses_** the cut (_S_, _V_ – _S_) if one of its endpoints belongs to _S_ and the other belongs to _V_ – _S_. A cut **_respects_** a set _A_ of edges if no edge in _A_ crosses the cut. An edge is a **_light edge_** crossing a cut if its weight is the minimum of any edge crossing the cut. There can be more than one light edge crossing a cut in the case of ties. More generally, we say that an edge is a **_light edge_** satisfying a given property if its weight is the minimum of any edge satisfying the property.

![[Art_P620.jpg]]

**_Theorem 21.1_**: Let _G_ = (_V_, _E_) be a connected, undirected graph with a real-valued weight function _w_ defined on _E_. Let _A_ be a subset of _E_ that is included in some minimum spanning tree for _G_, let (_S_, _V_ – _S_) be any cut of _G_ that respects _A_, and let (_u_, _v_) be a light edge crossing (_S_, _V_ – _S_). Then, edge (_u_, _v_) is safe for _A_.

**Corollary 21.2**: Let *G = (V, E)* be a connected, undirected graph with a real-valued weight function *w* defined on *E*. Let *A* be a subset of *E* that is included in some minimum spanning tree for *G*, and let $C = (V_C, E_C)$ be a connected component (tree) in the forest $G_A = (V, A)$. If *(u, v)* is a light edge connecting $C$ to some other component in $G_A$, then *(u, v)* is safe for *A*.

### **21.2    The algorithms of Kruskal and Prim**

In Kruskal's algorithm, the set _A_ is a forest whose vertices are all those of the given graph. The safe edge added to _A_ is always a lowest-weight edge in the graph that connects two distinct components. 
In Prim's algorithm, the set _A_ forms a single tree. The safe edge added to _A_ is always a lowest-weight edge connecting the tree to a vertex not in the tree. Both algorithms assume that the input graph is connected and represented by adjacency lists.

#### **Kruskal's algorithm**

Kruskal's algorithm finds a safe edge to add to the growing forest by finding, of all the edges that connect any two trees in the forest, an edge (_u_, _v_) with the lowest weight. Let $C_1$ and $C_2$ denote the two trees that are connected by (_u_, _v_). Since (_u_, _v_) must be a light edge connecting $C_1$ to some other tree, Corollary 21.2 implies that (_u_, _v_) is a safe edge for $C_1$. Kruskal's algorithm qualifies as a greedy algorithm because at each step it adds to the forest an edge with the lowest possible weight.

![[Art_P623.jpg]]

|     | MST-KRUSKAL(_G_, _w_)                                                    |
| --- | ------------------------------------------------------------------------ |
| 1   | _A_ = Ø                                                                  |
| 2   | **for** each vertex _v_ ∈ _G.V_                                          |
| 3   | `  `MAKE-SET(_v_)                                                        |
| 4   | create a single list of the edges in _G.E_                               |
| 5   | sort the list of edges into monotonically increasing order by weight _w_ |
| 6   | **for** each edge (_u_, _v_) taken from the sorted list in order         |
| 7   | `  `**if** FIND-SET(_u_) ≠ FIND-SET(_v_)                                 |
| 8   | `    `_A_ = _A_ ∪ {(_u_, _v_)}                                           |
| 9   | `    `UNION(_u_, _v_)                                                    |
| 10  | **return** _A_                                                           |

The total running time of Kruskal's algorithm is _O_(_E_ lg _E_).

#### **Prim's algorithm**

Prim's algorithm operates much like Dijkstra's algorithm for finding shortest paths in a graph. Prim's algorithm has the property that the edges in the set *A*  always form a single tree. The tree starts from an arbitrary root vertex *r* and grows until it spans all the vertices in *V*. Each step adds to the tree *A*  a light edge that connects *A*  to an isolated vertex—one on which no edge of *A*  is incident. This strategy qualifies as greedy since at each step it adds to the tree an edge that contributes the minimum amount possible to the tree's weight.

In order to efficiently select a new edge to add into tree A, the algorithm maintains a min-priority queue Q of all vertices that are not in the tree, based on a key attribute. For each vertex v, the attribute v.key is the minimum weight of any edge connecting v to a vertex in the tree, where by convention, v.key = ∞ if there is no such edge. The attribute v.π names the parent of v in the tree. The algorithm implicitly maintains the set _A_ from GENERIC-MST as
$$A = \{(v, v.π) : v ∈ V – \{r\} – Q\},$$
where we interpret the vertices in Q as forming a set.


![[Art_P625.jpg]]

|     | MST-PRIM(_G_, _w_, _r_)                          |                                            |
| --- | ------------------------------------------------ | ------------------------------------------ |
| 1   | **for** each vertex _u_ ∈ _G_._V_                |                                            |
| 2   | `  `_u.key_ = ∞                                  |                                            |
| 3   | `  `_u_.π = NIL                                  |                                            |
| 4   | `  `_r.key_ = 0                                  |                                            |
| 5   | `  `_Q_ = Ø                                      |                                            |
| 6   | **for** each vertex _u_ ∈ _G.V_                  |                                            |
| 7   | `  `INSERT(_Q_, _u_)                             |                                            |
| 8   | **while** _Q_ ≠ Ø                                |                                            |
| 9   | `  `_u_ = EXTRACT-MIN(_Q_)                       | // add _u_ to the tree                     |
| 10  | `  `**for** each vertex _v_ in _G.Adj_[_u_]      | // update keys of _u_'s non-tree neighbors |
| 11  | `  `**if** _v_ ∈ _Q_ and _w_(_u_, _v_) < _v.key_ |                                            |
| 12  | `    `_v_.π = _u_                                |                                            |
| 13  | `    `_v.key_ = _w_(_u_, _v_)                    |                                            |
| 14  | DECREASE-KEY(_Q_, _v_, _w_(_u_, _v_))            |                                            |

You can implement _Q_ with a binary min-heap. The total time for Prim's algorithm is _O_(_V_ lg _V_ + _E_ lg _V_) = _O_(_E_ lg _V_), which is asymptotically the same as for our implementation of Kruskal's algorithm.


## **22 Single-Source Shortest Paths**（8.5 最短路径）

 The input to a **shortest-paths problem** is a weighted, directed graph $G = (V, E)$, with a weight function $w : E \to \mathbb{R}$ mapping edges to real-valued weights. The **weight** $w(p)$ of path $p = \langle v_0, v_1, \dots, v_k \rangle$ is the sum of the weights of its constituent edges:
$$
w(p) = \sum_{i=1}^k w(v_{i-1}, v_i)
$$
We define the **shortest-path weight** $\delta(u, v)$ from $u$ to $v$ by
$$
\delta(u, v) =
\begin{cases}
\min\left\{ w(p) : u \stackrel{p}{\leadsto} v \right\} & \text{if there is a path from } u \text{ to } v, \\
\infty & \text{otherwise}
\end{cases}
$$
A **shortest path** from vertex $u$ to vertex $v$ is then defined as any path $p$ with weight $w(p) = \delta(u, v)$.

##### **Variants**

- **Single-destination shortest-paths problem
- **Single-pair shortest-path problem
- **All-pairs shortest-paths problem**

##### **Optimal substructure of a shortest path**

**_Lemma 22.1 (Subpaths of shortest paths are shortest paths)_**
Given a weighted, directed graph $G = (V, E)$ with weight function $w : E → ℝ$ let $p = 〈v_0, v_1, … , v_k〉$ be a shortest path from vertex $v_0$ to vertex $v_k$ and, for any _i_ and _j_ such that 0 ≤ _i_ ≤ _j_ ≤ _k_, let $p_{ij} = 〈v_i, v_{i+1}, … , v_j〉$ be the subpath of _p_ from vertex $v_i$ to vertex $v_j$. Then, $p_{ij}$ is a shortest path from $v_i$ to $v_j$.

##### **Negative-weight edges**

If the graph contains a negative-weight cycle reachable from _s_, however, shortest-path weights are not well defined. No path from _s_ to a vertex on the cycle can be a shortest path—you can always find a path with lower weight by following the proposed “shortest” path and then traversing the negative-weight cycle. If there is a negative-weight cycle on some path from _s_ to _v_, we define δ(_s_, _v_) = −∞.

##### Cycles

It cannot contain a negative-weight cycle. Nor can it contain a positive-weight cycle, since removing the cycle from the path produces a path with the same source and destination vertices and a lower path weight.

You can remove a 0-weight cycle from any path to produce another path whose weight is the same. Therefore, without loss of generality, assume that shortest paths have no cycles, that is, they are simple paths.

##### **Representing shortest paths**

Given a graph _G_ = (_V_, _E_), maintain for each vertex _v_ ∈ _V_ a **_predecessor_** _v_.π that is either another vertex or NIL.
In the midst of executing a shortest-paths algorithm, however, the π values might not indicate shortest paths. The **_predecessor subgraph_** $G_π = (V_π, E_π)$ induced by the π values is defined the same for single-source shortest paths as for breadth-first search in equations (20.2) and (20.3) on page 561:
$$
V_π = {v ∈ V : v.π ≠ NIL} ∪ {s},\quad
E_π = {(v.π, v) ∈ E : v ∈ V_π − {s}}.
$$
##### ⭐**Relaxation**

For each vertex _v_ ∈ _V_, the single-source shortest paths algorithms maintain an attribute _v.d_, which is an upper bound on the weight of a shortest path from source _s_ to _v_. We call _v.d_ a **_shortest-path estimate_**.

|     | INITIALIZE-SINGLE-SOURCE(_G_, _s_) |
| --- | ---------------------------------- |
| 1   | **for** each vertex _v_ ∈ _G.V_    |
| 2   | `  `_v.d_ = ∞                      |
| 3   | `  `_v_.π = NIL                    |
| 4   | _s.d_ = 0                          |

The process of ***relaxing*** an edge *(u, v)* consists of testing whether going through vertex *u* improves the shortest path to vertex *v* found so far and, if so, updating *v.d* and *v.π*.

|     | RELAX(_u_, _v_, _w_)                 |
| --- | ------------------------------------ |
| 1   | **if** _v.d_ > _u.d_ + _w_(_u_, _v_) |
| 2   | `  `_v.d_ = _u.d_ + _w_(_u_, _v_)    |
| 3   | `  `_v_.π = _u_                      |

Each algorithm in this chapter calls INITIALIZE-SINGLE-SOURCE and then repeatedly relaxes edges. Moreover, relaxation is the only means by which shortest-path estimates and predecessors change. The algorithms in this chapter differ in how many times they relax each edge and the order in which they relax edges. Dijkstra's algorithm and the shortest-paths algorithm for directed acyclic graphs relax each edge exactly once. The Bellman-Ford algorithm relaxes each edge |_V_| − 1 times.

##### **Properties of shortest paths and relaxation**

**Triangle inequality** (Lemma 22.10)
For any edge (_u_, _v_) ∈ _E_, we have δ(_s_, _v_) ≤ δ(_s_, _u_) + _w_(_u_, _v_).

**Upper-bound property** (Lemma 22.11)
We always have _v.d_ ≥ δ(_s_, _v_) for all vertices _v_ ∈ _V_, and once _v.d_ achieves the value δ(_s_, _v_), it never changes.

**No-path property** (Corollary 22.12)
If there is no path from _s_ to _v_, then we always have _v.d_ = δ(_s_, _v_) = ∞.

**Convergence property** (Lemma 22.14) （收敛性质）
If _s_ ⇝ _u_ → _v_ is a shortest path in _G_ for some _u_, _v_ ∈ _V_, and if _u.d_ = δ(_s_, _u_) at any time prior to relaxing edge (_u_, _v_), then _v.d_ = δ(_s_, _v_) at all times afterward.

**Path-relaxation property** (Lemma 22.15)
If $p = 〈v_0, v_1, … , v_k〉$ is a shortest path from $s = v_0$ to $v_k$, and the edges of _p_ are relaxed in the order $(v_0, v_1), (v_1, v_2), … , (v_{k_−1}, v_k)$, then $v_k.d = δ(s, v_k)$. This property holds regardless of any other relaxation steps that occur, even if they are intermixed with relaxations of the edges of _p_.

**Predecessor-subgraph property** (Lemma 22.17)
Once _v.d_ = δ(_s_, _v_) for all _v_ ∈ _V_, the predecessor subgraph is a shortest-paths tree rooted at _s_.

### **22.1    The Bellman-Ford algorithm**

The **_Bellman-Ford algorithm_** solves the single-source shortest-paths problem in the general case in which edge weights may be negative. Given a weighted, directed graph _G_ = (_V_, _E_) with source vertex _s_ and weight function _w_ : _E_ → ℝ, the Bellman-Ford algorithm returns a boolean value indicating whether there is a negative-weight cycle that is reachable from the source. If there is such a cycle, the algorithm indicates that no solution exists. If there is no such cycle, the algorithm produces the shortest paths and their weights.

|     | BELLMAN-FORD(_G_, _w_, _s_)              |
| --- | ---------------------------------------- |
| 1   | INITIALIZE-SINGLE-SOURCE(_G_, _s_)       |
| 2   | **for** _i_ = 1 **to** \|_G.V_\| − 1     |
| 3   | `  `**for** each edge (_u_, _v_) ∈ _G.E_ |
| 4   | `    `RELAX(_u_, _v_, _w_)               |
| 5   | **for** each edge (_u_, _v_) = _G.E_     |
| 6   | `  `**if** _v.d_ > _u.d_ + _w_(_u_, _v_) |
| 7   | `    `**return** FALSE                   |
| 8   | **return** TRUE                          |

![[Art_P636.jpg]]

**_Lemma 22.2_**
Let _G_ = (_V_, _E_) be a weighted, directed graph with source vertex _s_ and weight function _w_ : _E_ → ℝ, and assume that _G_ contains no negative-weight cycles that are reachable from _s_. Then, after the |_V_| − 1 iterations of the **for** loop of lines 2–4 of BELLMAN-FORD, _v.d_ = δ(_s_, _v_) for all vertices _v_ that are reachable from _s_.

**_Corollary 22.3_**
Let _G_ = (_V_, _E_) be a weighted, directed graph with source vertex _s_ and weight function _w_ : _E_ → ℝ. Then, for each vertex _v_ ∈ _V_, there is a path from _s_ to _v_ if and only if BELLMAN-FORD terminates with _v.d_ < ∞ when it is run on _G_.

![[Pasted image 20251214153857.png]]

### **22.2    Single-source shortest paths in directed acyclic graphs**

If the edges of a weighted dag _G_ = (_V_, _E_) are relaxed according to a topological sort of its vertices, it takes only Θ(_V_ + _E_) time to compute shortest paths from a single source.

|     | DAG-SHORTEST-PATHS(_G_, _w_, _s_)                                    |
| --- | -------------------------------------------------------------------- |
| 1   | topologically sort the vertices of _G_                               |
| 2   | INITIALIZE-SINGLE-SOURCE(_G_, _s_)                                   |
| 3   | **for** each vertex _u_ ∈ _G.V_, taken in topologically sorted order |
| 4   | `  `**for** each vertex _v_ in _G.Adj_[_u_]                          |
| 5   | `    `RELAX(_u_, _v_, _w_)                                           |

![[Art_P643.jpg]]

### **22.3    Dijkstra's algorithm**

Dijkstra's algorithm solves the single-source shortest-paths problem on a weighted, directed graph _G_ = (_V_, _E_), but it requires nonnegative weights on all edges: _w_(_u_, _v_) ≥ 0 for each edge (_u_, _v_) ∈ _E_.

You can think of Dijkstra's algorithm as generalizing breadth-first search to weighted graphs.

Dijkstra's algorithm maintains a set _S_ of vertices whose final shortest-path weights from the source _s_ have already been determined.

|     | DIJKSTRA(_G_, _w_, _s_)                        |
| --- | ---------------------------------------------- |
| 1   | INITIALIZE-SINGLE-SOURCE(_G_, _s_)             |
| 2   | _S_ = Ø                                        |
| 3   | _Q_ = Ø                                        |
| 4   | **for** each vertex _u_ ∈ _G.V_                |
| 5   | `  `INSERT(_Q, u_)                             |
| 6   | **while** _Q_ ≠ Ø                              |
| 7   | `  `_u_ = EXTRACT-MIN(_Q_)                     |
| 8   | `  `S = _S_ ∪ {u}                              |
| 9   | `  `**for** each vertex _v_ in _G.Adj_[_u_]    |
| 10  | `    `RELAX(_u_, _v_, _w_)                     |
| 11  | `    `**if** the call of RELAX decreased _v.d_ |
| 12  | `      `DECREASE-KEY(_Q_, _v_, _v.d_)          |

![[Art_P644.jpg]]

Because Dijkstra's algorithm always chooses the "lightest" or "closest" vertex in V − S to add to set S, you can think of it as using a greedy strategy.

##### **Analysis**

Just as in Prim's algorithm, the running time of Dijkstra's algorithm depends on the specific implementation of the min-priority queue _Q_.

By implementing the min-priority queue with a binary min-heap, the total running time is O((V + E) lg V), which is O(E lg V) in the typical case that |E| = Ω(V).

![[Pasted image 20251214153824.png]]

## **23        All-Pairs Shortest Paths**


















