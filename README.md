# B+ Tree C++ implementation

![BPlusTree](https://iq.opengenus.org/content/images/2018/06/b--search.jpg)

## Run the project
```
git clone https://github.com/ByJuanDiego/b-plus-tree.git
cd b-plus-tree
cmake -B<build-dir-name> -H.
cmake --build <build-dir-name> --target all
./<build-dir-name>/bplus_tree
```
replace ```<build-dir-name>``` with the desire build directory name

## Template parameters
```c++
template<
        int M, 
        typename K, 
        typename V, 
        typename Greater, 
        typename Index
> requires OrderConstraint<M>
class b_plus_tree;
```

- ```M```: integer that defines the order of the tree
> in a B+Tree of order $M$ it is true that, for each node, at most it has $M$ children and $M-1$ keys
- ```K```: this defines the key type used for inserting, searching and deleting
- ```V```: this type defines the record type to being accessed with the keys
- ```Greater``` A boolean function that is ```true``` when a key is greater than another and ```false``` otherwise.
- ```Index``` is a function type that recieves a record and returns the key to be used for indexing
- ```OrderConstraint<M>``` is a [concept](https://www.educative.io/answers/how-to-use-the-requires-clause-with-concepts-in-cpp-classes) that constraints the ```M``` value to integers greater than 2

## Member variables

```c++
int m; 
int n; 
int h;
node<K> *root;
Index index;
Greater greater;
```
- ```m```: minimum number of keys per node
> depends of the value of $M$, $m = \lceil \frac{M}{2} \rceil - 1$
- ```n```: number of records stored at leaf nodes
- ```h```: height of the tree
- ```root```: root node is a reference to the first level of the tree
> each node is reached from the root

## Member functions

|                                 Member Function                                 | Return Type |                                        Description                                        | Time Complexity |                                                                                        Notes                                                                                        |
|:-------------------------------------------------------------------------------:|:-----------:|:-----------------------------------------------------------------------------------------:|:---------------:|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
|                                  ```empty()```                                  | ```bool```  |              returns ```true``` if the tree is empty, ```false``` otherwhise              |     $O(1)$      |                                                                                          -                                                                                          |
|                                  ```clear()```                                  | ```void```  |                  frees up the memory and allows to continue using the B+                  |     $O(n)$      |                                                                                          -                                                                                          |
|                                  ```size()```                                   |  ```int```  |                                   returns ```this->n```                                   |     $O(1)$      |                                                                           ```size() = 0``` for empty tree                                                                           |
|                                 ```height()```                                  |  ```int```  |                                   returns ```this->h```                                   |     $O(1)$      |                                                                         ```height() = -1```  for empty tree                                                                         |
|                              ```insert(V value)```                              | ```void```  |                 insert a new record on a leaf node descending on the tree                 | $O(log_{M}(n))$ |                                                      occasionally splits one or more nodes recursively to increase the height                                                       |
| ```print(std::ostream &os, PrintFunction<V> disp_v, PrintFunction<K> disp_k)``` | ```void```  | traverse all the tree using [BFS](https://es.wikipedia.org/wiki/B%C3%BAsqueda_en_anchura) |     $O(n)$      | if ```V``` is not a [built-in type](https://en.cppreference.com/w/cpp/language/types) ```disp_v``` will be necessary, same with ```K``` (commonly, the key type is a build-in type) |

All the search operations returns an ```std::list<V>``` and are made on a $O(log_{M}(n) + k)$ time complexity, where $k$ is the cost of traversing the leaf nodes and could be different depending on the type of search, and the logarithmic function belongs to the cost of descending in the tree

|                                                                              Member Function                                                                              |                                 Return Description                                 |                                                                                                       Optional Parameters                                                                                                       |
|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:----------------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
|                                                                            ```search(K key)```                                                                            |          contains the $k$ records which index attribute equals ```key```           |                                                                                                                -                                                                                                                |
|                                                                            ```search_min()```                                                                             |          contains the $k$ records which index attribute are ***minimum***          |                                                                                                                -                                                                                                                |
 |                                                                            ```search_max()```                                                                             |         contains the $k$ records which index attribute are ***maxmimum***          |                                                                                                                -                                                                                                                |                                                                                 |
 |                                                                ```search_below(K max, bool include_max)```                                                                |      contains the $k$ records which index attribute are lesser than ```max```      |                                       the search returned ***do not include*** the superior limit for default, to include it, set the optional parameter ```include_max``` as ```true```                                        |
|                                                                ```search_above(K min, bool include_min)```                                                                |      contains the $k$ records which index attribute are grater than ```min```      |                                       the search returned ***do not include*** the inferior limit for default, to include it, set the optional parameter ```include_min``` as ```true```                                        |
|                                                  ```search_between(K min, K max, bool include_min, bool include_max)```                                                   | contains $k$ records which index attribute are between the ```min``` and ```max``` | the search returned ***includes*** both limits (inferior and superior) for default; to ***exclude*** one or both limits, set the ```include_min``` or ```include_max``` values to ```false``` depending on the desired semantic |

# Usage Cases

## Inicialization
```c++
std::function<int(transaction *)> index = [&](transaction *tx) -> int { return tx->amount; };
std::function<bool(int, int)> greater = [&](int a, int b) -> bool {return a > b;};
b_plus_tree<5, int, transaction *, std::function<bool(int, int)>> bPlusTree(index, greater);
```

 ```index``` function recieves a value and returns the attribute that will be used for indexing values

 ```greater``` is a function that, trivially, returns if an indexing value is greater than another
 
> - this last function do not need to be passed as parameter; in that case, the type is assigned to ```std::greater``` by default. If the indexing attribute is not a comparable type by default (which is not recomendable) a specialization of ```std::greater``` is necesary
> - for more information about ```std::greater``` specialization, visit [cppreference/greater](https://en.cppreference.com/w/cpp/utility/functional/greater)

## Querying
```c++
for (const transaction *i: bp.search_between(10, 97, true, false)) {
    std::cout << i->to_string() << std::endl;
}
```
This query returns all the ```transactions``` which ```amount``` value is between ```10 (inclusive)``` and ```97 (exclusive)``` in a non-decreasing order.
