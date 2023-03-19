# B+ Tree C++ implementation

![BPlusTree](https://iq.opengenus.org/content/images/2018/06/b--search.jpg)

## B+ Tree template type parameters
```c++
template<typename K, typename V, typename Index = std::function<K(V)>, typename Greater = std::greater<K>>
class b_plus_tree;
```

- ```K```: key type of the tree, this defines the criterion for inserting, searching and deleting
- ```V```: value type of the tree, this defines the type of the records to being accessed with the keys
- ```Index``` is a functor type that defines the type of function to be used for indexing
- ```Greater``` is a functor type that defines a condition that is true when a key is greater than another.

## B+ Tree (important) member variables

```c++
Index index;
```
- its a function that receives a record type and returns the attribute to be used for indexing the records

```c++
Greater greater;
```
- this function recieves two keys and returns ```true``` if the first is greater than the second and ```false``` otherwise

## B+ Tree member functions

All the search operations are made on a $O(log_{M}(n) + k)$ time complexity, where $k$ is the cost of traversing the leaf nodes and could be differente depending of the type of search, and the logarithmic cost belongs to the cost of descending in the tree

```c++
std::list<V> search(K key);
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute equals the ```key``` passed as parameter
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = key;```
- time complexity gets worse when having more records with the same ```key``` value


```c++
std::list<V> search_min();
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute are ***minimum***
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = min(T.a)```
- time complexity gets worse when having many records with the minimum key value


```c++
std::list<V> search_max();
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute are maxmimum
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = max(T.a)```
- time complexity gets worse when having many records with the maximum key value


```c++
std::list<V> search_below(KT max, bool includeMax);
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute are lesser than the key value passed as parameter, this key is the ```max``` value and determines the superior limit of the search
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a < max```
- the search returned ***do not include*** the superior limit for default, to include it, set the optional parameter ```include_max``` as ```true```
- time complexity gets worse when the ```max``` value gets closer to the maximum value in the tree


```c++
std::list<V> search_above(K min, bool include_min);
```
- this method returns an ```std::list``` that contains the $k$ records which index attribute are grater than the key passed as parameter, this key is the ```min``` value and determines the inferior limit of the search
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a > min;```
- the search returned ***do not include*** the inferior limit for default, to include it, set the optional parameter ```include_min``` as ```true```
- time complexity gets worse when the ```min``` value gets closer to the minimum value in the tree

```c++
std::list<V> search_between(K min, K max, bool include_min, bool include_max);
```
- this method returns an ```std::list``` that contains $k$ records which index attribute are between the ```min``` and ```max``` values passed as parameters
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE min <= T.a <= max;```
- the search returned ***includes*** both limits (inferior and superior) for default
- to ***exclude*** one or both limits, set the ```include_min``` or ```include_max``` values to ```false``` depending on the desired semantic
- time complexity gets worse when the difference between ```max``` and ```min``` is greater

# Usage Cases

## Inicialization
```c++
int M = 3;
std::function<int(transaction *)> index = [&](transaction *tx) -> int { return tx->ammount; };
std::function<bool(int, int)> greater = [&](int a, int b)->bool {return a > b;};

b_plus_tree<int, transaction *, decltype(greater)> bp(index, M, greater);
```

 ```index``` function recieves a value and returns the attribute that will be used for indexing values

 ```greater``` is a function that, trivially, returns if an indexing value is greater than another
 
> - this last function do not need to be passed as parameter; in that case, the type is assigned to ```std::greater``` by default. If the indexing attribute is not a comparable type by default (which is not recomendable) a specialization of ```std::greater``` is necesary
> - for more information about ```std::greater``` specialization, visit [cppreference/std::greater](https://en.cppreference.com/w/cpp/utility/functional/greater)
 
## Querying
```c++
for (const transaction *i: bp.search_between(10, 97, true, false)) {
    std::cout << i->to_string() << std::endl;
}
```
This query returns all the ```transactions``` which ```amount``` value is between ```10 (inclusive)``` and ```97 (exclusive)``` in a non-decreasing order.
