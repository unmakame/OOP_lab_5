#pragma once

#include <memory_resource>
#include <iterator>
#include <cassert>
#include <utility>
#include <cstddef>

template <typename T>
class List {
public:
    struct Node {
        T value;
        Node* next;
        template <typename... Args>
        explicit Node(Args&&... args)
            : value(std::forward<Args>(args)...), next(nullptr) {}
    };

    using allocator_type = std::pmr::polymorphic_allocator<Node>;

    explicit List(std::pmr::memory_resource* mr = std::pmr::get_default_resource());
    ~List() noexcept;

    List(const List&) = delete;
    List& operator=(const List&) = delete;
    template <typename... Args>
    void push_front(Args&&... args);

    void pop_front();
    void clear();

    T& front();
    const T& front() const;

    bool empty() const noexcept;
    std::size_t size() const noexcept;

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        explicit iterator(Node* n = nullptr) : node(n) {}

        reference operator*() const { return node->value; }
        pointer   operator->() const { return &node->value; }

        iterator& operator++() { node = node ? node->next : nullptr; return *this; }
        iterator  operator++(int) { auto tmp = *this; ++(*this); return tmp; }

        bool operator==(const iterator& other) const { return node == other.node; }
        bool operator!=(const iterator& other) const { return node != other.node; }

        Node* base() const { return node; }

    private:
        Node* node;
    };

    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;

        explicit const_iterator(const Node* n = nullptr) : node(n) {}

        reference operator*() const { return node->value; }
        pointer   operator->() const { return &node->value; }

        const_iterator& operator++() { node = node ? node->next : nullptr; return *this; }
        const_iterator  operator++(int) { auto tmp = *this; ++(*this); return tmp; }

        bool operator==(const const_iterator& other) const { return node == other.node; }
        bool operator!=(const const_iterator& other) const { return node != other.node; }

        const Node* base() const { return node; }

    private:
        const Node* node;
    };

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    allocator_type alloc;
    Node* head;
    std::size_t sz;
};

#include "list.ipp"