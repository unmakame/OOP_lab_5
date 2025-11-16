#include <iostream>
#include <memory_resource>

#include <list.h>
#include <allocator.h>

int main() {
    pmr_map_resource pool;
    std::pmr::polymorphic_allocator<int> pmr_alloc(&pool);
    int n;
    std::cin >> n;
    List<int> lst(&pool);
    
    for (int i = 0; i < n; i++) {
        int x;
        std::cin >> x;
        lst.push_front(x);
    }

    for (auto &v : lst) {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    return 0;
}
