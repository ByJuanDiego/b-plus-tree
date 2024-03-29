//
// Created by juandiego on 3/19/23.
//

#include "bplustree.hpp"
#include <iostream>

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
void b_plus_tree<M, K, V, Greater, Index>::non_full_insert(V value, node<K> *&node) {
    if (node->is_leaf) {
        auto *leaf = reinterpret_cast<leaf_node<K, V> *>(node);
        int i = leaf->num_keys;
        for (; i >= 1 && greater(leaf->keys[i - 1], index(value)); --i) {
            leaf->keys[i] = leaf->keys[i - 1];
            leaf->records[i] = leaf->records[i - 1];
        }

        leaf->keys[i] = index(value);
        leaf->records[i] = value;
        ++leaf->num_keys;
        return;
    }

    ::node<K> *&father = node;
    int j = 0;
    for (; j < father->num_keys && greater(index(value), father->keys[j]); ++j);

    ::node<K> *&child = father->children[j];
    if (child->num_keys < M) {
        non_full_insert(value, child);
    }
    if (child->num_keys == M) {
        child->split(father, j, M, m);
    }
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
K *b_plus_tree<M, K, V, Greater, Index>::remove(K key, node<K> *&node) {
    if (node->is_leaf) {
        auto *leaf = reinterpret_cast<leaf_node<K, V> *>(node);
        int searcher = leaf->locate_key(key, greater);

        if (searcher == -1) {
            throw std::runtime_error("The record to delete do not exists!");
        }

        K *predecessor = leaf->predecessor(searcher);
        leaf->remove_key(searcher);
        return predecessor;
    }

    ::node<K> *&father = node;
    int j = 0;
    for (; j < father->num_keys && greater(key, father->keys[j]); ++j);

    ::node<K> *&child = father->children[j];
    K *predecessor = remove(key, child);

    bool merge_required = false;
    if (child->num_keys < m) {
        ::node<K>* sibling = nullptr;

        if (child->is_leaf) {
            auto *leaf = reinterpret_cast<leaf_node<K, V> *>(child);

            if (father->has_children_at(j - 1, sibling) && sibling->num_keys > m) {
                auto *left_leaf = reinterpret_cast<leaf_node<K, V> *>(sibling);
                std::pair<K, V> to_transfer = left_leaf->pop_back();
                leaf->push_front(to_transfer.first, to_transfer.second);
                father->keys[j - 1] = *left_leaf->max_key();

                if (j < father->num_keys) {
                    father->keys[j] = *leaf->max_key();
                }
            }

            else if (father->has_children_at(j + 1, sibling) && sibling->num_keys > m) {
                auto *right_leaf = reinterpret_cast<leaf_node<K, V> *>(sibling);
                std::pair<K, V> to_transfer = right_leaf->pop_front();
                leaf->push_back(to_transfer.first, to_transfer.second);
                father->keys[j] = *leaf->max_key();
            }
            else {
                merge_required = true;
            }
        }
        else {
            auto *internal = reinterpret_cast<internal_node<K> *>(child);

            if (father->has_children_at(j - 1, sibling) && sibling->num_keys > m) {
                auto *left_internal = reinterpret_cast<internal_node<K> *>(sibling);
                std::pair<K, ::node<K> *> to_transfer = left_internal->pop_back();
                internal->push_front(father->keys[j - 1], to_transfer.second);
                father->keys[j - 1] = to_transfer.first;
            }
            else if (father->has_children_at(j + 1, sibling) && sibling->num_keys > m) {
                auto *right_internal = reinterpret_cast<internal_node<K> *>(sibling);
                std::pair<K, ::node<K> *> to_transfer = right_internal->pop_front();

                internal->push_back(father->keys[j], to_transfer.second);
                father->keys[j] = to_transfer.first;
            }
            else {
                merge_required = true;
            }
        }

        if (merge_required) {
            if (father->has_children_at(j - 1, sibling)) {
                this->merge(sibling, child, father, (j - 1));
            }
            else if (father->has_children_at((j + 1), sibling)) {
                if (predecessor && equals(key, father->keys[j])) {
                    father->keys[j] = *predecessor;
                    delete predecessor;
                    predecessor = nullptr;
                }
                this->merge(child, sibling, father, j);
            }
        }
    }

    if (predecessor && j < father->num_keys && equals(key, father->keys[j])) {
        father->keys[j] = *predecessor;
        delete predecessor;
        predecessor = nullptr;
    }

    return predecessor;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>void
b_plus_tree<M, K, V, Greater, Index>::merge(node<K> *&to_merge, node<K> *&to_add, node<K> *&father, int to_merge_pos) {

    if (to_merge->is_leaf) {
        auto to_merge_leaf = reinterpret_cast<leaf_node<K, V>*>(to_merge);
        auto to_add_leaf = reinterpret_cast<leaf_node<K, V>*>(to_add);

        if (to_add_leaf->num_keys > 0) {
            for (int i = 0; i < to_add->num_keys; i++) {
                to_merge_leaf->push_back(to_add_leaf->keys[i], to_add_leaf->records[i]);
            }
            father->keys[to_merge_pos + 1] = *to_merge_leaf->max_key();
        }
        else {
            father->keys[to_merge_pos + 1] = father->keys[to_merge_pos];
        }

        to_merge_leaf->next_leaf = to_add_leaf->next_leaf;
        if (to_add_leaf->next_leaf) {
            to_add_leaf->next_leaf->prev_leaf = to_merge_leaf;
        }

        delete to_add_leaf;

        for (int i = to_merge_pos; i < (father->num_keys - 1); ++i) {
            father->keys[i] = father->keys[(i + 1)];
            father->children[(i + 1)] = father->children[(i + 2)];
        }

        --father->num_keys;
    }
    else {
        auto to_merge_internal = reinterpret_cast<internal_node<K> *>(to_merge);
        auto to_add_internal = reinterpret_cast<internal_node<K> *>(to_add);

        to_merge_internal->push_back(father->keys[to_merge_pos], to_add_internal->children[0]);

        for (int i = 0; i < to_add_internal->num_keys; ++i) {
            to_merge_internal->push_back(to_add_internal->keys[i], to_add_internal->children[i + 1]);
        }

        delete to_add_internal;

        for (int i = to_merge_pos; i < (father->num_keys - 1); ++i) {
            father->keys[i] = father->keys[(i + 1)];
            father->children[(i + 1)] = father->children[(i + 2)];
        }

        --father->num_keys;
    }


}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Index, typename Greater>
requires OrderConstraint<M>
leaf_node<K, V> *b_plus_tree<M, K, V, Index, Greater>::search_node(node<K> *node, K key) {
    if (!node) {
        return nullptr;
    }
    while (!node->is_leaf) {
        int i = 0;
        for (; i < node->num_keys && greater(key, node->keys[i]); ++i);
        node = node->children[i];
    }

    return reinterpret_cast<leaf_node<K, V> *>(node);
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
b_plus_tree<M, K, V, Greater, Index>::b_plus_tree(Index index, Greater greater)
        : n(0), m(int(std::ceil(M / 2.0)) - 1), h(-1), root(nullptr), index{index}, greater{greater} {
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
b_plus_tree<M, K, V, Greater, Index>::~b_plus_tree() {
    if (!root) {
        return;
    }
    this->root->kill_self();
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
int b_plus_tree<M, K, V, Greater, Index>::size() {
    return this->n;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
bool b_plus_tree<M, K, V, Greater, Index>::empty() {
    return !this->root;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
void b_plus_tree<M, K, V, Greater, Index>::clear() {
    if (!root) {
        return;
    }
    this->root->kill_self();
    this->h = -1;
    this->n = 0;
    this->root = nullptr;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
int b_plus_tree<M, K, V, Greater, Index>::height() {
    return this->h;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
bool b_plus_tree<M, K, V, Greater, Index>::find(K key) {
    leaf_node<K, V> *leaf = search_node(root, key);
    if (!leaf) {
        return false;
    }

    for (int i = 0; i < leaf->num_keys; ++i) {
        if (equals(key, leaf->keys[i])) {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
void b_plus_tree<M, K, V, Greater, Index>::insert(V value) {
    if (!root) {
        root = new leaf_node<K, V>(M, true);
        ++this->h;
    }
    if (root->num_keys < M) {
        non_full_insert(value, root);
    }
    if (root->num_keys == M) {
        node<K> *old_root = this->root;
        root = new internal_node<K>(M);
        root->children[0] = old_root;
        old_root->split(root, 0, M, m);
        ++this->h;
    }
    ++this->n;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
void b_plus_tree<M, K, V, Greater, Index>::remove(K key) {
    if (this->empty()) {
        return;
    }

    this->remove(key, root);

    if (root->num_keys == 0) {
        root = root->children[0];
        --this->h;
    }
    --this->n;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Index, typename Greater>
requires OrderConstraint<M>
std::list<V> b_plus_tree<M, K, V, Index, Greater>::search(K key) {
    std::list<V> values;
    leaf_node<K, V> *leaf = search_node(root, key);

    while (leaf) {
        for (int i = 0; i < leaf->num_keys; ++i) {
            if (greater(leaf->keys[i], key))
                return values;
            if (!greater(key, leaf->keys[i]))
                values.push_back(leaf->records[i]);
        }
        leaf = leaf->next_leaf;
    }
    return values;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Index, typename Greater>
requires OrderConstraint<M>
std::list<V> b_plus_tree<M, K, V, Index, Greater>::search_min() {
    std::list<V> values;
    if (this->empty())
        return values;

    node<K> *node = root;
    while (!node->is_leaf)
        node = node->children[0];

    auto *leaf = reinterpret_cast<leaf_node<K, V> *>(node);

    K min = leaf->keys[0];
    while (leaf) {
        for (int j = 0; j < leaf->num_keys; ++j) {
            if (greater(leaf->keys[j], min))
                return values;
            values.push_front(leaf->records[j]);
        }
        leaf = leaf->next_leaf;
    }
    return values;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Index, typename Greater>
requires OrderConstraint<M>
std::list<V> b_plus_tree<M, K, V, Index, Greater>::search_max() {
    std::list<V> values;
    if (this->empty())
        return values;

    node<K> *node = root;
    while (!node->is_leaf)
        node = node->children[node->num_keys];

    auto *leaf = reinterpret_cast<leaf_node<K, V> *>(node);

    K max = leaf->keys[leaf->num_keys - 1];
    while (leaf) {
        for (int j = (leaf->num_keys - 1); j >= 0; --j) {
            if (greater(max, leaf->keys[j]))
                return values;
            values.push_front(leaf->records[j]);
        }
        leaf = leaf->prev_leaf;
    }
    return values;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Index, typename Greater>
requires OrderConstraint<M>
std::list<V> b_plus_tree<M, K, V, Index, Greater>::search_below(K upper_bound, bool include_max) {
    std::list<V> search;
    leaf_node<K, V> *leaf = search_node(root, upper_bound);
    auto include_condition = ([&](K key) {
        return include_max ? (!greater(key, upper_bound)) : (greater(upper_bound, key));
    });

    while (leaf) {
        for (int i = (leaf->num_keys - 1); i >= 0; --i)
            if (include_condition(leaf->keys[i]))
                search.push_front(leaf->records[i]);
        leaf = leaf->prev_leaf;
    }
    return search;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Index, typename Greater>
requires OrderConstraint<M>
std::list<V> b_plus_tree<M, K, V, Index, Greater>::search_above(K lower_bound, bool include_min) {
    std::list<V> search;
    leaf_node<K, V> *leaf = search_node(root, lower_bound);
    auto include_condition = ([&](K key) {
        return include_min ? (!greater(lower_bound, key)) : (greater(key, lower_bound));
    });

    while (leaf) {
        for (int i = 0; i < leaf->num_keys; ++i)
            if (include_condition(leaf->keys[i]))
                search.push_back(leaf->records[i]);
        leaf = leaf->next_leaf;
    }
    return search;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Index, typename Greater>
requires OrderConstraint<M>
std::list<V>
b_plus_tree<M, K, V, Index, Greater>::search_between(K lower_bound, K upper_bound, bool include_min, bool include_max) {
    std::list<V> search;
    leaf_node<K, V> *leaf = search_node(root, lower_bound);
    auto stop_condition = ([&](K key) -> bool {
        return include_max ? (greater(key, upper_bound)) : (!greater(upper_bound, key));
    });
    auto include_condition = ([&](K key) -> bool {
        return include_min ? (!greater(lower_bound, key)) : (greater(key, lower_bound));
    });

    while (leaf) {
        for (int i = 0; i < leaf->num_keys; ++i) {
            if (stop_condition(leaf->keys[i]))
                return search;
            if (include_condition(leaf->keys[i]))
                search.push_back(leaf->records[i]);
        }
        leaf = leaf->next_leaf;
    }
    return search;
}

//-----------------------------------------------------------------------------

template<int M, typename K, typename V, typename Greater, typename Index>
requires OrderConstraint<M>
void b_plus_tree<M, K, V, Greater, Index>::print(std::ostream &os, Print<V> print_value, Print<K> print_key) {
    if (!root) {
        os << "[]";
        return;
    }

    int level = 0;
    std::queue<std::pair<node<K> *, int>> queue;

    queue.push({root, level});
    while (!queue.empty()) {
        auto [node, l] = queue.front();
        queue.pop();

        if (l > level) {
            ++level;
            os << std::endl;
        }

        if (node->is_leaf) {
            auto *leaf_node = reinterpret_cast<::leaf_node<K, V> *>(node);
            leaf_node->print(os, print_key, print_value);
            os << ((leaf_node->next_leaf) ? " <==> " : "");
        } else {
            auto *internal_node = reinterpret_cast<::internal_node<K> *>(node);
            internal_node->print(os, print_key);
        }

        if (node->is_leaf) {
            continue;
        }

        for (int i = 0; i <= node->num_keys; ++i) {
            queue.push({node->children[i], l + 1});
        }
    }
}

//-----------------------------------------------------------------------------
