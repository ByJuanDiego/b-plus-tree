//
// Created by juandiego on 3/19/23.
//

#include "bplustree.hpp"

template<int M, typename K, typename V, typename Greater, typename Index>
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

template<int M, typename K, typename V, typename Index, typename Greater>
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

template<int M, typename K, typename V, typename Greater, typename Index>
b_plus_tree<M, K, V, Greater, Index>::b_plus_tree(Index index, Greater greater) :
        n(0), m(int(std::ceil(M / 2.0)) - 1),
        h(-1), root(nullptr), index{index}, greater{greater} {
}

template<int M, typename K, typename V, typename Greater, typename Index>
b_plus_tree<M, K, V, Greater, Index>::~b_plus_tree() {
    if (!root) {
        return;
    }
    this->root->kill_self();
}

template<int M, typename K, typename V, typename Greater, typename Index>
int b_plus_tree<M, K, V, Greater, Index>::size() {
    return this->n;
}

template<int M, typename K, typename V, typename Greater, typename Index>
bool b_plus_tree<M, K, V, Greater, Index>::empty() {
    return !this->root;
}

template<int M, typename K, typename V, typename Greater, typename Index>
void b_plus_tree<M, K, V, Greater, Index>::clear() {
    if (!root) {
        return;
    }
    this->root->kill_self();
    this->h = -1;
    this->n = 0;
    this->root = nullptr;
}

template<int M, typename K, typename V, typename Greater, typename Index>
int b_plus_tree<M, K, V, Greater, Index>::height() {
    return this->h;
}

template<int M, typename K, typename V, typename Greater, typename Index>
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

template<int M, typename K, typename V, typename Index, typename Greater>
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

template<int M, typename K, typename V, typename Index, typename Greater>
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

template<int M, typename K, typename V, typename Index, typename Greater>
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

template<int M, typename K, typename V, typename Index, typename Greater>
std::list<V> b_plus_tree<M, K, V, Index, Greater>::search_below(K max, bool include_max) {
    std::list<V> search;
    leaf_node<K, V> *leaf = search_node(root, max);
    auto include_condition = ([&](K key) {
        return include_max ? (!greater(key, max)) : (greater(max, key));
    });

    while (leaf) {
        for (int i = (leaf->num_keys - 1); i >= 0; --i)
            if (include_condition(leaf->keys[i]))
                search.push_front(leaf->records[i]);
        leaf = leaf->prev_leaf;
    }
    return search;
}

template<int M, typename K, typename V, typename Index, typename Greater>
std::list<V> b_plus_tree<M, K, V, Index, Greater>::search_above(K min, bool include_min) {
    std::list<V> search;
    leaf_node<K, V> *leaf = search_node(root, min);
    auto include_condition = ([&](K key) {
        return include_min ? (!greater(min, key)) : (greater(key, min));
    });

    while (leaf) {
        for (int i = 0; i < leaf->num_keys; ++i)
            if (include_condition(leaf->keys[i]))
                search.push_back(leaf->records[i]);
        leaf = leaf->next_leaf;
    }
    return search;
}

template<int M, typename K, typename V, typename Index, typename Greater>
std::list<V> b_plus_tree<M, K, V, Index, Greater>::search_between(K min, K max, bool include_min, bool include_max) {
    std::list<V> search;
    leaf_node<K, V> *leaf = search_node(root, min);
    auto stop_condition = ([&](K key) -> bool {
        return include_max ? (greater(key, max)) : (!greater(max, key));
    });
    auto include_condition = ([&](K key) -> bool {
        return include_min ? (!greater(min, key)) : (greater(key, min));
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

template<int M, typename K, typename V, typename Greater, typename Index>
void b_plus_tree<M, K, V, Greater, Index>::print(std::ostream &os, PrintFunction<V> disp_v, PrintFunction<K> disp_k) {
    os <<
       "{" <<
       "\n    \"records\": " << this->size() <<
       "\n    \"height\": " << this->height() <<
       "\n    \"M\": " << M <<
       "\n    \"m\": " << this->m <<
       "\n}\n";

    if (!root) {
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
            leaf_node->print(os, disp_k, disp_v);
            os << ((leaf_node->next_leaf) ? " <==> " : "");
        } else {
            auto *internal_node = reinterpret_cast<::internal_node<K> *>(node);
            internal_node->print(os, disp_k);
        }

        if (node->is_leaf) {
            continue;
        }

        for (int i = 0; i <= node->num_keys; ++i) {
            queue.push({node->children[i], l + 1});
        }
    }
}
