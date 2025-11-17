#pragma once
#include <memory_resource>
#include <map>
#include <vector>
#include <mutex>
#include <cstdlib>
#include <new>
#include <iostream>

class pmr_map_resource : public std::pmr::memory_resource {
public:
    pmr_map_resource() = default;
    ~pmr_map_resource() noexcept;

    size_t allocated_count() const;
    size_t free_count() const;

protected:
    void* do_allocate(size_t bytes, size_t alignment) override;
    void  do_deallocate(void* p, size_t bytes, size_t alignment) override;
    bool  do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

private:
    mutable std::mutex mut_;
    std::map<void*, size_t> allocated_;
    std::map<size_t, std::vector<void*>> free_map_;
};

