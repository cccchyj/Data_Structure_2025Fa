# Lab 4 : Scheduling to maximize profit

**Course:** CS20009.04 Data Structure
**Name:** Yijia Chen
**Student Number:** 24300240127
**Date:** November 25, 2025


<br>

## Implementation

The aim is to maximize the profit of a set of jobs carried out in limited time. To obtain profit, each job selected should be completed before its deadline.

The problem can be solved by dynamic programming. It exhibits the property of optimal substructure. Consider job $j$ with processing time $t_j$, a profit $p_j$, and a deadline $d_j$. If the job is included in the schedule that yield the most profit, then the schedule also contains the optimal schedule before the starting time of job $j$, $t-t_j$, where $t$ is the time when job $j$ is done. Otherwise, we can copy-and-paste a better subschedule to fit in the time before $t-t_j$ and obtain a greater profit by time $t$.

Therefore, we use the vector `dp` to memoize the maximum profit of a schedule that can be completed before time $t$ for all $0 \leq t \leq \max\{d_j\}$. The scale of the problem correspond to the time $t$. The solution to a larger problem can be obtained by considering the subproblem, whose answer has been recorded in the vector `dp`.

<!-- @import "scheduling.cpp" -->

<br>

Note that greedy algorithm does not apply to this problem. The local optimality cannot lead to a global optimal solution. The following is a counter example:

```
j t  p  d
1 5 900 5
2 2 300 2
3 2 300 3
4 2 300 4
5 2 300 5
```

The optimal schedule by time 4 is, comoplete task 2 during time 1-2, and then complete task 5 during time 3-4. However, when we consider the schedule by time 5, completing task 1 alone leads to the most profit, because its profit is high enough to make up for the huge consumption of time.


<br>

## Analysis

The algorithm consists of the following steps:

- Sort the jobs by their deadlines. It takes $O(n\lg n)$ time.
- Find the max deadline of all the jobs. It takes $O(n)$ time.
- Initialize the vector `dp`. It takes $O(1)$ time.
- Dynamic programming takes a bottom-up approach. The nested loop takes $O(n \times \max\{d_j\})$ time in the worst case. According to the project specification, the processing time is smaller than the number of jobs, so the time complexity is $O(n^2)$.
- Find the maximum profit by traversing all the elements in `dp`. It takes $O(n)$ time.
- Reconstruct the schedule. We start from the latest task and then backtrack to find the schedule. It takes $O(n)$ time. Reversing the `schedule` vector also takes $O(n)$ time.

It can be concluded that the total time complexity is $O(n^2)$.

<br>

## Project Specification

Suppose you have one machine and a set of n jobs $a_1, a_2, ... a_n$, to process on that machine. Each job $a_j$ has a processing time $t_j$, a profit $p_j$, and a deadline $d_j$. The machine can process only one job at a time, and job $a_j$ must run uninterruptedly for $t_j$ consecutive time units. If job $a_j$ is completed by its deadline $d_j$, you receive a profit $p_j$, but if it is completed after its deadline, you receive a profit of 0. Give an algorithm to find the schedule that obtains the maximum amount of profit, assuming that all processing times are integers between 1 and n. what is the running time of you algorithm?

Grading.
(1)Algorithm and implemented code (including three use cases)(60%). 
(2)Efficiency of the algorithm (20%).
(3)Document (20%)