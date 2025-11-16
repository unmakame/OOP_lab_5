#pragma once
#include <memory_resource>
#include <iterator>
#include <cassert>

template <typename T>
class List {
public:
    struct Node {
        T value;
        Node* next;
        template <typename... Args>
        Node(Args&&... args) : value(std::forward<Args>(args)...), next(nullptr) {}
    };

    using allocator_type = std::pmr::polymorphic_allocator<Node>;

    explicit List(std::pmr::memory_resource* mr = std::pmr::get_default_resource());
    ~List();

    List(const List&) = delete;
    List& operator=(const List&) = delete;

    template <typename... Args>
    void push_front(Args&&... args);

    void pop_front();
    void clear();

    T& front();
    const T& front() const;

    bool empty() const noexcept;
    size_t size() const noexcept;

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        iterator(Node* n = nullptr) : node(n) {}

        reference operator*() const { return node->value; }
        pointer   operator->() const { return &node->value; }

        iterator& operator++() { node = node->next; return *this; }
        iterator  operator++(int) { auto tmp = *this; ++(*this); return tmp; }

        bool operator==(const iterator& other) const { return node == other.node; }
        bool operator!=(const iterator& other) const { return node != other.node; }

    private:
        Node* node;
    };

    iterator begin();
    iterator end();

private:
    allocator_type alloc;
    Node* head;
    size_t sz;
};

#include "List.ipp"
