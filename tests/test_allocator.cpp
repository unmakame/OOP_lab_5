#include <gtest/gtest.h>
#include "allocator.h"
#include <vector>
#include <string>
#include <memory>

class PMRAllocatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        resource = std::make_unique<pmr_map_resource>();
    }

    void TearDown() override {
        resource.reset();
    }

    std::unique_ptr<pmr_map_resource> resource;
};

TEST_F(PMRAllocatorTest, BasicAllocation) {
    std::pmr::memory_resource& mr = *resource;
    
    void* ptr1 = mr.allocate(100, 8);
    void* ptr2 = mr.allocate(200, 16);
    void* ptr3 = mr.allocate(50, 4);
    
    EXPECT_EQ(resource->allocated_count(), 3);
    EXPECT_EQ(resource->free_count(), 0);
    
    mr.deallocate(ptr1, 100, 8);
    mr.deallocate(ptr2, 200, 16);
    
    EXPECT_EQ(resource->allocated_count(), 1);
    EXPECT_EQ(resource->free_count(), 2);
    
    mr.deallocate(ptr3, 50, 4);
}

TEST_F(PMRAllocatorTest, MemoryReuse) {
    std::pmr::memory_resource& mr = *resource;
    
    void* ptr1 = mr.allocate(100, 8);
    mr.deallocate(ptr1, 100, 8);
    
    EXPECT_EQ(resource->free_count(), 1);
    
    void* ptr2 = mr.allocate(100, 8);
    
    EXPECT_EQ(resource->allocated_count(), 1);
    EXPECT_EQ(resource->free_count(), 0);
    
    mr.deallocate(ptr2, 100, 8);
}

TEST_F(PMRAllocatorTest, DifferentSizes) {
    std::pmr::memory_resource& mr = *resource;
    
    void* small = mr.allocate(16, 8);
    void* medium = mr.allocate(64, 8);
    void* large = mr.allocate(256, 8);
    
    EXPECT_EQ(resource->allocated_count(), 3);
    
    mr.deallocate(small, 16, 8);
    mr.deallocate(medium, 64, 8);
    mr.deallocate(large, 256, 8);
    
    EXPECT_EQ(resource->free_count(), 3);
}

TEST_F(PMRAllocatorTest, Alignment) {
    std::pmr::memory_resource& mr = *resource;
    
    void* ptr1 = mr.allocate(100, 8);
    void* ptr2 = mr.allocate(100, 16);
    void* ptr3 = mr.allocate(100, 32);
    
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 16, 0);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr3) % 32, 0);
    
    mr.deallocate(ptr1, 100, 8);
    mr.deallocate(ptr2, 100, 16);
    mr.deallocate(ptr3, 100, 32);
}

TEST_F(PMRAllocatorTest, StressTest) {
    std::pmr::memory_resource& mr = *resource;
    
    const int NUM_ALLOCATIONS = 100;
    std::vector<void*> pointers;
    
    for (int i = 0; i < NUM_ALLOCATIONS; ++i) {
        size_t size = (i % 10 + 1) * 16;
        void* ptr = mr.allocate(size, 8);
        pointers.push_back(ptr);
    }
    
    EXPECT_EQ(resource->allocated_count(), NUM_ALLOCATIONS);
    
    for (int i = 0; i < NUM_ALLOCATIONS; i += 2) {
        size_t size = (i % 10 + 1) * 16;
        mr.deallocate(pointers[i], size, 8);
    }
    
    EXPECT_EQ(resource->allocated_count(), NUM_ALLOCATIONS / 2);
    EXPECT_EQ(resource->free_count(), NUM_ALLOCATIONS / 2);
    
    for (int i = 1; i < NUM_ALLOCATIONS; i += 2) {
        size_t size = (i % 10 + 1) * 16;
        mr.deallocate(pointers[i], size, 8);
    }
    
    EXPECT_EQ(resource->allocated_count(), 0);
    EXPECT_EQ(resource->free_count(), NUM_ALLOCATIONS);
}

TEST_F(PMRAllocatorTest, PMRContainers) {
    std::pmr::vector<int> vec{resource.get()};
    
    EXPECT_EQ(resource->allocated_count(), 0);
    
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }
    
    EXPECT_GT(resource->allocated_count(), 0);
    
    vec.clear();
    
    std::pmr::string str1{"Hello", resource.get()};
    std::pmr::string str2{"World", resource.get()};
    
    EXPECT_GT(resource->allocated_count(), 0);
}

TEST_F(PMRAllocatorTest, IsEqual) {
    pmr_map_resource resource2;
    
    std::pmr::memory_resource& mr1 = *resource;
    std::pmr::memory_resource& mr2 = resource2;
    
    EXPECT_TRUE(mr1.is_equal(mr1));
    EXPECT_FALSE(mr1.is_equal(mr2));
}

TEST_F(PMRAllocatorTest, MemoryLeakCheck) {
    {
        pmr_map_resource local_resource;
        std::pmr::memory_resource& mr = local_resource;
        
        void* ptr1 = mr.allocate(100, 8);
        void* ptr2 = mr.allocate(200, 16);
        
        mr.deallocate(ptr1, 100, 8);
    }
}

TEST_F(PMRAllocatorTest, LargeAllocations) {
    std::pmr::memory_resource& mr = *resource;
    
    const size_t LARGE_SIZE = 1024 * 1024; // 1MB
    void* large_ptr = mr.allocate(LARGE_SIZE, 8);
    
    EXPECT_NE(large_ptr, nullptr);
    EXPECT_EQ(resource->allocated_count(), 1);
    
    mr.deallocate(large_ptr, LARGE_SIZE, 8);
    EXPECT_EQ(resource->allocated_count(), 0);
}