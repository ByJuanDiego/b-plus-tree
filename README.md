# B+ Tree C++ implementation

## B+ Tree template type parameters
```c++
template<typename KT, typename KV, typename Index = std::function<KT(KV)>>
class BPlusTree;
```

- ```KT```: key type of the tree, this defines the criterion for inserting, searching and deleting
- ```KV```: value type of the tree, this defines the type of the records to being accessed with the keys
- ```Index``` is a functor type that defines the type of function to be used for indexing

## B+ Tree member variables

```c++
int M;
```
- defines the order of the tree
- in a B+Tree of order $M$ it is true that, for each node, at most it has $M$ children and $M-1$ keys

```c++
int m;
```
- defines the minimun number of keys for each node
- it depends of the value of $M$, $m = \lceil \frac{M}{2} \rceil - 1$

```c++
int n;
```
- its an $O(1)$ access to the number of records stored at leaf nodes

```c++
int h;
```
- its an $O(1)$ access to the height of the tree

```c++
Node<KT> *root;
```
- root node is a reference to the first level of the tree; each node is reached from the root

```c++
Index f;
```
- its a function that receives a record type and returns the attribute to be used for indexing the records


## B+ Tree member functions

All the search operations are made on a $O(log_{M}(n) + k)$ time complexity, where $k$ is the cost of traversing the leaf nodes and could be differente depending of the type of search and the logarithmic cost belongs to the cost of descend in the tree

```c++
std::list<KV> search(KT key);
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute equals the ```key``` passed as parameter
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = key;```
- time complexity gets worse when having more records with the same ```key``` value


```c++
std::list<KV> searchMin();
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute are ***minimum***
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = (SELECT min(T1.a) FROM T T1)```
- time complexity gets worse when having many records with the minimum key value


```c++
std::list<KV> searchMax();
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute are maxmimum
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = (SELECT max(T1.a) FROM T T1)```
- time complexity gets worse when having many records with the maximum key value


```c++
std::list<KV> searchBelow(KT max, bool includeMax);
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute are lesser than the key value passed as parameter, this key is the ```max``` value and determines the superior limit of the search
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = (SELECT min(T1.a) FROM T T1)```
- the search returned ***do not includes*** the superior limit for default, to include it, set the optional parameter ```includeMax``` as ```true```
- time complexity gets worse when the ```max``` value gets closer to the maximum value in the tree


```c++
std::list<KV> searchAbove(KT min, bool includeMin);
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute are grater than the key passed as parameter, this key is the ```min``` value and determines the inferior limit of the search
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a > min;```
- the search returned ***do not includes*** the inferior limit for default, to include it, set the optional parameter ```includeMin``` as ```true```
- time complexity gets worse when the ```min``` value gets closer to the minimum value in the tree

```c++
std::list<KV> searchBetween(KT min, KT max, bool includeMin, bool includeMax);
```
- this method returns an ```std::list``` that contains $k$ records which index attribute are between the ```min``` and ```max``` values passed as parameters
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE min <= T.a <= max;```
- the search returned ***includes*** both limits (inferior and superior) for default
- to ***exclude*** one or both limits, set the ```includeMin``` or ```includeMax``` values to ```false``` depending of the desired semantic
- time complexity gets worse when the difference between ```max``` and ```min``` is greater
