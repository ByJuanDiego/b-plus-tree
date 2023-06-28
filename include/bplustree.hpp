//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_BPLUSTREE_DEF_H
#define BPLUS_TREE_BPLUSTREE_DEF_H

#include <cmath>
#include <queue>
#include <list>

#include "node.cpp"

#ifndef MINIMUM_ORDER
#define MINIMUM_ORDER 3
#endif

/// Template callback to display types that overloads operator<< and built-in types
template<typename Printable>
Print<Printable> print = [](std::ostream &os, const Printable &p) { os << p; };

/// Constraint to limit the values of M to integers greater than 2 before runtime
template<int M>
concept OrderConstraint = M >= MINIMUM_ORDER;

/**
 * B+ Tree index for queries with support for non-primary keys indexing
 *
 * In a B+ Tree of order M it is true that, for each node, at most it has M children and M-1 keys
 *    if at any moment that clause is not true, nodes are splitted recursively from down to up until the clause is true again or the root node is splitted
 *    in that last case, the pointer to the root is assigned to a new node and old root becomes the first child (children[0]) of the new root
 *
 * @tparam M the order of the tree
 * @tparam K the type of the key
 * @tparam V the type of data stored at leaf nodes
 * @tparam Greater callable used to define when a key is greater than another
 * @tparam Index callable used to obtain the index of a value
 */
template<
        int M,
        typename K,
        typename V,
        typename Greater = std::greater<K>,
        typename Index = std::function<K(V)>
> requires OrderConstraint<M>
class b_plus_tree {
private:

    int m;              //< Minimum number of keys per node
    int n;              //< Number of records stored at leaf nodes
    int h;              //< Height of the tree
    node<K> *root;      //< Is a reference to the first level of the tree
    Index index;        //< Receives a value and returns it corresponding key
    Greater greater;    //< Receives two keys and returns true the first one is greater than the second

    /** Recursive function that descends the tree to insert a new record at a leaf node
     *
     * The base case is when a leaf node is reached, there is when the new record is inserted in order
     * Then, the function returns to the previous call and makes sure that his previous visited child do not exceeds the
     *     maximum number of keys, if not, the node is splitted. The process is repeated until the first call is reached
     *
     * @param value the record to be inserted
     * @param node the current node
     */
    void non_full_insert(V value, node<K> *&node);

    K *remove(K key, node<K> *&node);

    /** Iterative function that returns the first leaf node that stores key equal to `key`
     *
     * If the B+ is empty, returns nullptr
     * If the key do not exists, returns the first leaf node that stores a key greater than `key`
     */
    leaf_node<K, V> *search_node(node<K> *node, K key);

    inline bool equals(K a, K b) {
        return !greater(a, b) && !greater(b, a);
    }

public:

    /// Constructs a B+ Tree
    explicit b_plus_tree(Index index, Greater greater = Greater());

    /// Destructs a B+ Tree
    ~b_plus_tree();

    /// Returns `n`
    int size();

    /// Returns `true` if no value is stored and `false` otherwise
    bool empty();

    /// Frees up the memory in post-order and allows to continue using the B+
    void clear();

    /// Returns `h`
    int height();

    /// Returns `true` if `key` is at a leaf node and `false` otherwise
    bool find(K key);

    /**
     * Inserts a new record on a leaf node descending on the tree
     *
     * Internally calls `non_full_insert`
     * Occasionally splits one or more nodes recursively
     * When the `root` node is splited, `h` increases in 1
     */
    void insert(V value);

    /// Removes a record
    void remove(K key);

    /// Returns the records which index attribute equals `key`
    std::list<V> search(K key);

    /// Returns the records which index attribute are minimum
    std::list<V> search_min();

    /// Returns the records which index attribute are maximum
    std::list<V> search_max();

    /// Returns the records which index attribute are lesser than `upper_bound`
    std::list<V> search_below(K upper_bound, bool include_max = false);

    /// Returns the records which index attribute are greater than `lower_bound`
    std::list<V> search_above(K lower_bound, bool include_min = false);

    /// Returns the records which index attribute are between `lower_bound` and `upper_bound`
    std::list<V> search_between(K lower_bound, K upper_bound, bool include_min = true, bool include_max = true);

    /**
     * Prints all the B+ Tree
     *
     * Traverses all the tree using BFS
     *
     * @param os std::ostream object to output stream the hash table in console or load it to a file
     * @param print_value function parameter that defines how to print a `V` type
     * @param print_key function parameter that defines how to print a `K` type
     */
    void print(std::ostream &os, Print<V> print_value = ::print<V>, Print<K> print_key = ::print<K>);

};


#endif //BPLUS_TREE_BPLUSTREE_DEF_H
