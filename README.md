# B+ Tree C++ implementation

## B+ Tree class methods

All the search operations are made on a $O(log_{m}(n) + k)$ 

```c++
std::list<KV> BPlusTree<KT, KV, Index>::search(KT key);
```
- this method returns an ```std::list``` that contains the *k* registers which index attribute equals the ```key``` passed as parameter
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = key;```
- time complexity gets worse when having more registers with the same ```key``` value


```c++
std::list<KV> BPlusTree<KT, KV, Index>::searchMin();
```
- this method returns an ```std::list``` that contains the *k* registers which index attribute are ***minimum***
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = (SELECT min(T1.a) FROM T T1)```
- time complexity gets worse when having many registers with the minimum key value


```c++
std::list<KV> BPlusTree<KT, KV, Index>::searchMax();
```
- this method returns an ```std::list``` that contains the *k* registers which index attribute are maxmimum
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = (SELECT max(T1.a) FROM T T1)```
- time complexity gets worse when having many registers with the maximum key value


```c++
std::list<KV> BPlusTree<KT, KV, Index>::searchBelow(KT max, bool includeMax);
```
- this method returns an ```std::list``` that contains the *k* registers which index attribute are lesser than the key value passed as parameter, this key is the ```max``` value and determines the superior limit of the search
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a = (SELECT min(T1.a) FROM T T1)```
- the search returned ***do not includes*** the superior limit for default, to include it, set the optional parameter ```includeMax``` as ```true```
- time complexity gets worse when the limit max value gets closer to the maximum value in the tree


```c++
std::list<KV> BPlusTree<KT, KV, Index>::searchAbove(KT min, bool includeMin);
```
- this method returns an ```std::list``` that contains the *k* registers which index attribute are grater than the key passed as parameter, this key is the ```min``` value and determines the inferior limit of the search
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE T.a > min;```
- the search returned ***do not includes*** the inferior limit for default, to include it, set the optional parameter ```includeMin``` as ```true```
- time complexity gets worse when the limit min value gets closer to the minimum value in the tree

```c++
std::list<KV> BPlusTree<KT, KV, Index>::searchBetween(KT min, KT max, bool includeMin, bool includeMax);
```
- this method returns an ```std::list``` that contains *k* registers which index attribute are between the ```min``` and ```max``` values passed as parameters
- this method is semantically equivalent to the sql query: ```SELECT * FROM T WHERE min <= T.a <= max;```
- the search returned ***includes*** both limits (inferior and superior) for default
- to ***exclude*** one or both limits, set the ```includeMin``` or ```includeMax``` values to ```false``` depending of the desired semantic
- time complexity gets worse when the difference between max and min is greater
