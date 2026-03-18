#include "allocator.h"

pmr_map_resource::~pmr_map_resource() {
    std::lock_guard<std::mutex> lk(mut_);

    for (auto& p : allocated_) {
        std::free(p.first);
    }
    for (auto& kv : free_map_) {
        for (void* p : kv.second) {
            std::free(p);
        }
    }
}

size_t pmr_map_resource::allocated_count() const {
    std::lock_guard<std::mutex> lk(mut_);
    return allocated_.size();
}

size_t pmr_map_resource::free_count() const {
    std::lock_guard<std::mutex> lk(mut_);
    size_t total = 0;
    for (auto& kv : free_map_) total += kv.second.size();
    return total;
}

void* pmr_map_resource::do_allocate(size_t bytes, size_t alignment) {
    std::lock_guard<std::mutex> lk(mut_);

    auto it = free_map_.find(bytes);
    if (it != free_map_.end() && !it->second.empty()) {
        void* blk = it->second.back();
        it->second.pop_back();
        allocated_.emplace(blk, bytes);
        return blk;
    }

    void* ptr = nullptr;
    int r = posix_memalign(&ptr, alignment, bytes);
    if (r != 0) ptr = std::malloc(bytes);
    if (!ptr) throw std::bad_alloc();

    allocated_.emplace(ptr, bytes);
    return ptr;
}

void pmr_map_resource::do_deallocate(void* p, size_t bytes, size_t) {
    std::lock_guard<std::mutex> lk(mut_);

    auto it = allocated_.find(p);
    if (it == allocated_.end()) {
        std::cerr << "Warning: deallocating unknown pointer\n";
        std::free(p);
        return;
    }

    allocated_.erase(it);
    free_map_[bytes].push_back(p);
}

bool pmr_map_resource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}
