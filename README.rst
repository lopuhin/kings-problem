Chess kings problem
===================

Written for the quiz http://zealint.ru/maxflow-kings-1dwalks-comp.html , later improved by зщл.

The problem
-----------
Calculate the number of ways to place N^2 kings on the 2N x 2N board, so that they don't attack each other, for the largest possible N. Solutions for N up to 11 were given http://oeis.org/A018807 , the winner solved the problem for N=20

Approach
--------
First I came up an idea of a recurcive algorithm, that uses the solution from the previos N. But before implementing it I decided to search for other alternatives, and found an unpublished paper by Donald Knuth http://www-cs-faculty.stanford.edu/~uno/papers/nkc.tex.gz
The algorithm proposed there seemed to be of the same (maybe better?) asymptotic complexity, and was better formulated, so I used it instead of mine.

The algorithm
-------------
Number of kings placements equals number of full directed paths in the graph, consisting of vetices (a, b, S), where 0 ≤ a ≤ N, 0 ≤ b ≤ N, S ⊆ {1, ..., N}. There is a directed arc from (a, b, S) to (a, b + 1, T) iff S ⊆ T and (if a < j < a' and j ∈ S then j + 1 ∈ T, if a > j > a' and j + 1 ∈ S then j ∈ T).
By full I mean paths (0, 0, ∅) → (a', 1, S1) → ... → (an, n, Sn).

I applied a number of obvious optimizations to the algorithm:
 * having calculated the number of paths to vertices for layer b = n, it is easy to calculated them for b = n+1 - for each vertice B in the layer b = n+1 just sum the number of paths to vertices in the previous layer, that are connected with B
 * represent sets as integer numbers: m is in set iff m-th bit is set
 * given the constraints on sets S and T, determine the minimal set M that satisfies the constraints, then sum the number of paths for seach pair (a', M), and use this number to calculate the number of paths to vertices in the next layer
 * cache supersets of all sets S

Later зщл removed caching of supersets, optimized their calculation, and removed repeating summation in function assign_n_paths_in_current_state.

Implementation 
---------------
* kings.py - naive implementation, that enumerates all valid placements, runs in reasonable time up to N = 3
* knuth.py - (optimized) implementation of algorithm suggested by Knuth here http://www-cs-faculty.stanford.edu/~uno/papers/nkc.tex.gz in Python 
* knuth.cpp - parallel implementation of the same algorithm in C

In C version I used GMP for long arithmetics, and OpenMP to scale it to multiplce cores. An interesting fact is that on single core, Python version run under PyPy was only 1.9 times slower than the C version, complied with gcc -O3. But scaling C version was much simpler due to the magic of OpenMP (basically, added only 4 lines).

I calculated the number of kings placements for N = 19 on Amazon EC2 High-CPU instance (High-CPU Extra Large Instance 7 GB of memory, 20 EC2 Compute Units (8 virtual cores with 2.5 EC2 Compute Units each), 1690 GB of local instance storage, 64-bit platform) in 2 hours 50 minutes. The winner calculated number of placements for N = 20 in 18 hours, using five 4-core machines and a different algorithm.
