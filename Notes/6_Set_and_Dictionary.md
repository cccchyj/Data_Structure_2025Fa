# 第6章 集合与字典

## 6.1 集合及其表示

位向量的实现：当 $n$ 是一个不大的整数时，则可用二进位 (0,1) 数组（bit vector) 来实现集合。
有序链表的实现：
![[Pasted image 20251213133130.png]]

## 6.2 并查集与等价类

并查集是一种简单的用途广泛的集合，也称 disjoint set。它支持以下3种操作。

1. `Union(Root1,Root2)`：把子集 `Root2` 并入集合 `Root1` 中。要求 `Root1` 与 `Root2` 互不相交，否则不执行合并。
2. `Find(x)`：搜索单元素 x 所在的集合，并返回该集合的名字。
3. `UFSets(s)`：将并查集（用 `UFSets` 命名）中 s 个元素初始化为 s 个只有一个单元素的子集。

![[Pasted image 20251213135144.png]]

为了避免产生退化的树，一种改进的方法是**先判断两集合中元素的个数**，如果以 `i` 为根的树中的结点个数少于以 `j` 为根的树中的结点个数，则让 `j` 成为 `i` 的父，否则，让 `i` 成为 `j` 的父。

![[Pasted image 20251213135056.png]]

为了进一步改进树的性能，可以使用如下的**折叠规则**来压缩路径：如果  `j`  是从 `i` 到根的路径上的一个结点，并且 `parent[j]` $\neq$ `root[i]`，则把 `parent[j]` 置为 `root[i]`。使用折叠规则完成单个搜索，所需时间大约增加一倍。但是，它能减少在最坏情况下完成一系列搜索操作所需的时间。

![[Pasted image 20251213135446.png]]


### 6.2.2 并查集的应用：等价类划分

例如，给定集合 $S = \{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11\}$，及如下等价对： $0\equiv4$，$3\equiv1$，$6\equiv10$，$8\equiv9$，$7\equiv4$，$6\equiv8$，$3\equiv5$，$2\equiv11$，$11\equiv0$ 。

![[Pasted image 20251213135802.png]]

#### **An application of disjoint-set data structures**

One of the many applications of disjoint-set data structures arises in determining the connected components of an undirected graph.

![[Art_P557.jpg]]

## 6.3 字典

字典可以用线性表的方式组织，线性表可以用基于数组的存储表示，也可以用基于链表的存储表示，因而可以考虑用顺序搜索或用折半搜索的方法来搜索要求的元素。考虑到搜索效率，还可以采用二叉搜索树或多路搜索树的方式组织字典。本节介绍典型的 3 种字典的组织方式：线性表、跳表和散列表。

## 6.4 跳表

在该结构上有一组有层次的链，0 级链是包含所有元素的有序链表，其第 $2^1, 2×2^1, 3×2^1\cdots$ 个结点链接起来形成 1 级链，故 1 级链是 0 级链的一个子集；$2^2, 2×2^2, 3×2^2\cdots$ 个结点链接起来形成 2 级链，以此类推，第 $i$ 级链所包含的元素是 $i-1$ 级链的子集。在图 6.12(c)中，$i$ 级链上所有元素都在 $i-1$ 级链上。一个有 $n$ 个元素的跳表理想情况下的链级数为 $\lceil \log_2 n \rceil$，即跳表的最高级数为 $\lceil \log_2 n \rceil - 1$。 

![[Pasted image 20251213140037.png]]

## 6.5 散列

> 算法导论

### 11.1  **Direct-address tables**

Direct addressing is a simple technique that works well when the universe _U_ of keys is  reasonably small. Suppose that an application needs a dynamic set in which each element has a distinct key drawn from the universe _U_ = {0, 1, …, _m_ − 1}, where _m_ is not too large.

To represent the dynamic set, you can use an array, or **_direct-address table_**, denoted by _T_[0 : _m_ − 1], in which each position, or **_slot_**, corresponds to a key in the universe _U_. Slot _k_ points to an element in the set with key _k_. If the set contains no element with key _k_, then _T_[_k_] = NIL.

![[Art_P378.jpg]]

The downside of direct addressing is apparent: if the universe U is large or infinite, storing a table T of size |U| may be impractical, or even impossible, given the memory available on a typical computer. Furthermore, the set K of keys actually stored may be so small relative to U that most of the space allocated for T would be wasted.
### 11.2 Hash tables

The storage requirement reduces to Θ(|_K_|) while maintaining the benefit that searching for an element in the hash table still requires only _O_(1) time.

We use a **_hash function_** _h_ to compute the slot number from the key _k_, so that the element goes into slot _h_(_k_). The hash function _h_ maps the universe _U_ of keys into the slots of a **_hash table_** _T_[0 : _m_ − 1] :
$$h : U → \{0, 1, …, m − 1\},$$
where the size _m_ of the hash table is typically much less than |_U_|. We say that an element with key _k_ **_hashes_** to slot _h_(_k_), and we also say that _h_(_k_) is the **_hash value_** of key _k_.

There is one hitch, namely that two keys may hash to the same slot. We call this situation a **_collision_**. One idea is to make h appear to be "random," thus avoiding collisions or at least minimizing their number.

![[Art_P379.jpg]]

#### **Independent uniform hashing**

An "ideal" hashing function h would have, for each possible input k in the domain U, an output h(k) that is an element randomly and independently chosen uniformly from the range {0, 1, …, m − 1}. Once a value h(k) is randomly chosen, each subsequent call to h with the same input k yields the same output h(k).

#### **Collision resolution by chaining**

![[Art_P380.jpg]]

**_Chaining_**: each nonempty slot points to a linked list, and all the elements that hash to the same slot go into that slot’s linked list. Slot _j_ contains a pointer to the head of the list of all stored elements with hash value _j_. If there are no such elements, then slot _j_ contains NIL.

#### **Analysis of hashing with chaining**

Given a hash table _T_ with _m_ slots that stores _n_ elements, we define the **_load factor_** _α_ for _T_ as _n_/_m_, that is, the average number of elements stored in a chain.

We assume that _O_(1) time suffices to compute the hash value _h_(_k_), so that the time required to search for an element with key _k_ depends linearly on the length _n_ _h_(_k_) of the list _T_[_h_(_k_)].

**_Theorem 11.1_**: In a hash table in which collisions are resolved by chaining, an unsuccessful search takes Θ(1 + _α_) time on average, under the assumption of independent uniform hashing.

**_Theorem 11.2_**: In a hash table in which collisions are resolved by chaining, a successful search takes Θ(1 + _α_) time on average, under the assumption of independent uniform hashing.

Since insertion takes _O_(1) worst-case time and deletion takes _O_(1) worst-case time when the lists are doubly linked (assuming that the list element to be deleted is known, and not just its key), we can support all dictionary operations in _O_(1) time on average.

The analysis in the preceding two theorems depends only on two essential properties of independent uniform hashing: uniformity (each key is equally likely to hash to any one of the _m_ slots), and independence (so any two distinct keys collide with probability 1/_m_).

### **11.3 Hash functions**

Two ad hoc approaches for creating hash functions: hashing by division and hashing by multiplication. Although these methods work well for some sets of input keys, they are limited because they try to provide a single fixed hash function that works well on any data—an approach called ***static hashing***.

We then see that provably good average-case performance for any data can be obtained by designing a suitable family of hash functions and choosing a hash function at random from this family at runtime, independent of the data to be hashed. The approach we examine is called random hashing.
#### What makes a good hash function?

A good hash function satisfies (approximately) the assumption of independent uniform hashing: each key is equally likely to hash to any of the _m_ slots, independently of where any other keys have hashed to.

#### **Keys are integers, vectors, or strings**

To begin, we assume that keys are short nonnegative integers.

#### **11.3.1    Static hashing**

##### **The division method**

The **_division method_** for creating hash functions maps a key _k_ into one of _m_ slots by taking the remainder of _k_ divided by _m_. That is, the hash function is
$$h(k) = k \mod m.$$
The division method may work well when _m_ is a prime not too close to an exact power of 2.
> 否则 $m = 2^r$ 时，哈希函数值仅由 $k$ 的部分位决定。

##### The multiplication method

The general ***multiplication method*** for creating hash functions operates in two steps. First, multiply the key $k$ by a constant $A$ in the range $0 < A < 1$ and extract the fractional part of $kA$. Then, multiply this value by m and take the floor of the result. That is, the hash function is
$$h(k) = ⌊m (kA \mod 1)⌋,$$
where “$kA \mod 1$” means the fractional part of $kA$, that is, $kA − ⌊kA⌋$. The general multiplication method has the advantage that the value of m is not critical and you can choose it independently of how you choose the multiplicative constant $A$.

##### **⭐The multiply-shift method**

$$h_a(k) = (k a \mod 2^w) \ggg (w - \ell)$$

![[Art_P385.jpg]]
> $h_a(k)$ 为乘积的中间几位，由 $k$ 的高位部分和低位部分共同决定，确保 $k$ 值微小的变化能引起 $h_a(k)$ 较大的变化。
#### **11.3.2    Random hashing**

##### Universal (good, theoretically):
$$ h_{ab}(k) = (((ak + b) \mod p) \mod m) $$
- **Hash Family** $\mathcal{H}(p,m) = \{ h_{ab} \mid a,b \in \{0, \dots, p-1\} \text{ and } a \neq 0 \}$
- Parameterized by a fixed prime $p > u$, with $a$ and $b$ chosen from range $\{0, \dots, p-1\}$
- $\mathcal{H}$ is a **Universal family**: $\underset{h \in \mathcal{H}}{\text{Pr}} \left\{ h(k_i) = h(k_j) \right\} \leq 1/m \quad \forall k_i \neq k_j \in \{0, \dots, u-1\}$
- Why is universality useful? Implies short chain lengths! (in expectation)
- $X_{ij}$ indicator random variable over $h \in \mathcal{H}$: $X_{ij} = 1$ if $h(k_i) = h(k_j)$, $X_{ij} = 0$ otherwise
- Size of chain at index $h(k_i)$ is random variable $X_i = \sum_j X_{ij}$
- Expected size of chain at index $h(k_i)$:
$$
\underset{h \in \mathcal{H}}{\mathbb{E}} \left\{ X_i \right\} = \underset{h \in \mathcal{H}}{\mathbb{E}} \left\{ \sum_j X_{ij} \right\} = \sum_j \underset{h \in \mathcal{H}}{\mathbb{E}} \left\{ X_{ij} \right\} = 1 + \sum_{j \neq i} \underset{h \in \mathcal{H}}{\mathbb{E}} \left\{ X_{ij} \right\}
$$
$$
= 1 + \sum_{j \neq i} \left( (1) \underset{h \in \mathcal{H}}{\text{Pr}} \left\{ h(k_i) = h(k_j) \right\} + (0) \underset{h \in \mathcal{H}}{\text{Pr}} \left\{ h(k_i) \neq h(k_j) \right\} \right)
$$
$$
\leq 1 + \sum_{j \neq i} 1/m = 1 + (n-1)/m
$$
- Since $m = \Omega(n)$, load factor $\alpha = n/m = O(1)$, so $O(1)$ in expectation!

### **11.4    Open addressing**（开放寻址法）

This section describes open addressing, a method for collision resolution that, unlike chaining, does not make use of storage outside of the hash table itself. In **_open addressing_**, all elements occupy the hash table itself.

Of course, you could use chaining and store the linked lists inside the hash table, in the otherwise unused hash-table slots, but the advantage of open addressing is that it avoids pointers altogether. Instead of following pointers, you compute the sequence of slots to be examined.

To perform insertion using open addressing, successively examine, or ***probe***, the hash table until you find an empty slot in which to put the key. Instead of being fixed in the order $0, 1, …, m − 1$ (which implies a $Θ(n)$ search time), the sequence of positions probed depends upon the key being inserted. To determine which slots to probe, the hash function includes the probe number (starting from 0) as a second input. Thus, the hash function becomes
$$h : U × {0, 1, …, m − 1} → {0, 1, …, m − 1}.$$
Open addressing requires that for every key $k$, the ***probe sequence*** $〈h(k, 0), h(k, 1), …, h(k, m − 1)〉$ be a permutation of $〈0, 1, …, m − 1〉$, so that every hash-table position is eventually considered as a slot for a new key as the table fills up.

![[Pasted image 20251213211747.png]]
![[Pasted image 20251213211806.png]]

![[Pasted image 20251213210840.png]]

Deletion from an open-address hash table is tricky. When you delete a key from slot $q$, it would be a mistake to mark that slot as empty by simply storing `NIL` in it. If you did, you might be unable to retrieve any key $k$ for which slot $q$ was probed and found occupied when $k$ was inserted. One way to solve this problem is by marking the slot, storing in it the special value `DELETED` instead of `NIL`. 
Using the special value `DELETED`, however, means that search times no longer depend on the load factor $α$, and for this reason chaining is frequently selected as a collision resolution technique when keys must be deleted.

#### Linear probing（线性探查）

Given an ordinary hash function $h' : U \to \{0, 1, \dots, m-1\}$, which we refer to as an **auxiliary hash function**, the method of linear probing uses the hash function
$$h(k,i) = (h'(k) + i) \mod m$$
for $i = 0, 1, \dots, m-1$. Given key $k$, we first probe $T[h'(k)]$, i.e., the slot given by the auxiliary hash function. We next probe slot $T[h'(k) + 1]$, and so on up to slot $T[m-1]$. Then we wrap around to slots $T[0], T[1], \dots$ until we finally probe slot $T[h'(k) - 1]$. Because the initial probe determines the entire probe sequence, there are only $m$ distinct probe sequences.

Linear probing is easy to implement, but it suffers from a problem known as **primary clustering**. Long runs of occupied slots build up, increasing the average search time. Clusters arise because an empty slot preceded by $i$ full slots gets filled next with probability $(i+1)/m$. Long runs of occupied slots tend to get longer, and the average search time increases.

![[Pasted image 20251213213613.png]]
#### Quadratic probing（二次探查）

Quadratic probing uses a hash function of the form
$$h(k,i) = (h'(k) + c_1 i + c_2 i^2) \mod m \ , \tag{11.5}$$
where $h'$ is an auxiliary hash function, $c_1$ and $c_2$ are positive auxiliary constants, and $i = 0, 1, \dots, m-1$. The initial position probed is $T[h'(k)]$; later positions probed are offset by amounts that depend in a quadratic manner on the probe number $i$. This method works much better than linear probing, but to make full use of the hash table, the values of $c_1, c_2,$ and $m$ are constrained. Also, if two keys have the same initial probe position, then their probe sequences are the same, since $h(k_1,0) = h(k_2,0)$ implies $h(k_1,i) = h(k_2,i)$. This property leads to a milder form of clustering, called **secondary clustering**. As in linear probing, the initial probe determines the entire sequence, and so only $m$ distinct probe sequences are used.

![[Pasted image 20251213213639.png]]

> 随机性不够，本质上并没有改善线性探查中的一次群集/堆积问题。

#### Double hashing

Double hashing offers one of the best methods available for open addressing because the permutations produced have many of the characteristics of randomly chosen permutations. **Double hashing** uses a hash function of the form
$$h(k,i) = (h_1(k) + i h_2(k)) \mod m \ ,$$
where both $h_1$ and $h_2$ are auxiliary hash functions. The initial probe goes to position $T[h_1(k)]$; successive probe positions are offset from previous positions by the amount $h_2(k)$, modulo $m$. Thus, the probe sequence here depends in two ways upon the key $k$, since the initial probe position $h_1(k)$, the step size $h_2(k)$, or both, may vary.

In order for the entire hash table to be searched, the value $h_2(k)$ must be relatively prime to the hash-table size $m$.  A convenient way to ensure this condition is to let $m$ be an exact power of  $2$ and to design $h_2$ so that it always produces an odd number. Another way is to let $m$ be prime and to design $h_2$ so that it always returns a positive integer less than $m$.

When $m$ is prime or an exact power of $2$, double hashing produces $Θ(m^2)$ probe sequences, since each possible $(h_1(k), h_2(k))$ pair yields a distinct probe sequence.

#### **Analysis of open-address hashing**

The bound proven, of $1/(1 − α) = 1 + α + α^2 + α^3 + ⋯$, has an intuitive interpretation. The first probe always occurs. With probability approximately $α$, the first probe finds an occupied slot, so that a second probe happens. With probability approximately $α^2$, the first two slots are occupied so that a third probe ensues, and so on.

**_Theorem 11.6_**: Given an open-address hash table with load factor _α_ = _n_/_m_ < 1, the expected number of probes in an unsuccessful search is at most 1/(1 − _α_), assuming independent uniform permutation hashing and no deletions.

**_Corollary 11.7_**: Inserting an element into an open-address hash table with load factor _α_, where _α_ < 1, requires at most 1/(1 − _α_) probes on average, assuming independent uniform permutation hashing and no deletions.

**_Theorem 11.8_**: Given an open-address hash table with load factor _α_ < 1, the expected number of probes in a successful search is at most
$$\frac{1}{\alpha} \ln \frac{1}{1-\alpha},$$
assuming independent uniform permutation hashing with no deletions and assuming that each key in the table is equally likely to be searched for.

### 6.5.5 散列表分析

![[Pasted image 20251213214657.png]]
