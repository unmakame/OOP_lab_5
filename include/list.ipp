#pragma once    
#include <algorithm> 

template <typename T>
List<T>::List(std::pmr::memory_resource* mr)
    : alloc(mr), head(nullptr), sz(0) {}

template <typename T>
List<T>::~List() {
    clear();
}

template <typename T>
template <typename... Args>
void List<T>::push_front(Args&&... args) {
    Node* n = alloc.allocate(1);
    new (n) Node(std::forward<Args>(args)...);
    n->next = head;
    head = n;
    ++sz;
}

template <typename T>
void List<T>::pop_front() {
    if (!head) return;
    Node* n = head;
    head = head->next;
    n->~Node();
    alloc.deallocate(n, 1);
    --sz;
}

template <typename T>
void List<T>::clear() {
    Node* cur = head;
    while (cur) {
        Node* next = cur->next;
        cur->~Node();
        alloc.deallocate(cur, 1);
        cur = next;
    }
    head = nullptr;
    sz = 0;
}

template <typename T>
T& List<T>::front() {
    assert(head);
    return head->value;
}

template <typename T>
const T& List<T>::front() const {
    assert(head);
    return head->value;
}

template <typename T>
bool List<T>::empty() const noexcept {
    return head == nullptr;
}

template <typename T>
std::size_t List<T>::size() const noexcept {
    return sz;
}

template <typename T>
typename List<T>::iterator List<T>::begin() {
    return iterator(head);
}

template <typename T>
typename List<T>::iterator List<T>::end() {
    return iterator(nullptr);
}

template <typename T>
typename List<T>::const_iterator List<T>::begin() const {
    return const_iterator(head);
}

template <typename T>
typename List<T>::const_iterator List<T>::end() const {
    return const_iterator(nullptr);
}

template <typename T>
typename List<T>::const_iterator List<T>::cbegin() const {
    return const_iterator(head);
}

template <typename T>
typename List<T>::const_iterator List<T>::cend() const {
    return const_iterator(nullptr);
}