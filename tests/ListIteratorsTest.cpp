#include <gtest/gtest.h>
#include "list.h"
#include "allocator.h"
#include <vector>
#include <string>

class ListPMRTest : public ::testing::Test {
protected:
    void SetUp() override {
        resource = std::make_unique<pmr_map_resource>();
    }

    void TearDown() override {
        resource.reset();
    }

    std::unique_ptr<pmr_map_resource> resource;
};

TEST_F(ListPMRTest, BasicListOperations) {
    std::cout << "🧪 Testing basic list operations..." << std::endl;
    
    List<int> lst{resource.get()};
    EXPECT_TRUE(lst.empty());
    EXPECT_EQ(lst.size(), 0);

    // Добавляем элементы
    lst.push_front(3);
    lst.push_front(2);
    lst.push_front(1);

    EXPECT_FALSE(lst.empty());
    EXPECT_EQ(lst.size(), 3);
    EXPECT_EQ(lst.front(), 1);

    std::cout << "   List size: " << lst.size() << std::endl;
    std::cout << "   Memory blocks: " << resource->allocated_count() << std::endl;
}

TEST_F(ListPMRTest, MemoryAllocationTracking) {
    std::cout << "🧪 Testing memory allocation tracking..." << std::endl;
    
    EXPECT_EQ(resource->allocated_count(), 0);
    EXPECT_EQ(resource->free_count(), 0);

    {
        List<int> lst{resource.get()};
        
        lst.push_front(1);
        EXPECT_EQ(resource->allocated_count(), 1);
        
        lst.push_front(2);
        EXPECT_EQ(resource->allocated_count(), 2);
        
        lst.push_front(3);
        EXPECT_EQ(resource->allocated_count(), 3);

        std::cout << "   After 3 push_front: " << resource->allocated_count() << " blocks" << std::endl;
    }

    // После уничтожения списка память должна быть освобождена
    EXPECT_EQ(resource->allocated_count(), 0);
    EXPECT_GT(resource->free_count(), 0);
    
    std::cout << "   After list destruction: " << resource->free_count() << " cached blocks" << std::endl;
}

TEST_F(ListPMRTest, IteratorFunctionality) {
    std::cout << "🧪 Testing iterator functionality..." << std::endl;
    
    List<int> lst{resource.get()};
    lst.push_front(3);
    lst.push_front(2);
    lst.push_front(1);

    // Forward iteration
    auto it = lst.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, lst.end());

    // Const iteration
    const List<int>& const_lst = lst;
    auto cit = const_lst.begin();
    EXPECT_EQ(*cit, 1);
    
    std::cout << "   Iteration test passed" << std::endl;
}

TEST_F(ListPMRTest, RangeBasedForLoop) {
    std::cout << "🧪 Testing range-based for loop..." << std::endl;
    
    List<int> lst{resource.get()};
    lst.push_front(3);
    lst.push_front(2);
    lst.push_front(1);

    std::vector<int> result;
    for (const auto& val : lst) {
        result.push_back(val);
    }

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 3);

    std::cout << "   Range-based for loop works correctly" << std::endl;
}

TEST_F(ListPMRTest, PopFrontMemory) {
    std::cout << "🧪 Testing pop_front memory management..." << std::endl;
    
    List<int> lst{resource.get()};
    
    lst.push_front(3);
    lst.push_front(2);
    lst.push_front(1);

    size_t initial_allocated = resource->allocated_count();
    std::cout << "   Before pop_front: " << initial_allocated << " blocks" << std::endl;

    lst.pop_front();
    EXPECT_EQ(resource->allocated_count(), initial_allocated - 1);
    EXPECT_EQ(lst.front(), 2);

    lst.pop_front();
    EXPECT_EQ(resource->allocated_count(), initial_allocated - 2);
    EXPECT_EQ(lst.front(), 3);

    std::cout << "   After 2 pop_front: " << resource->allocated_count() << " blocks" << std::endl;
}

TEST_F(ListPMRTest, ClearReleasesMemory) {
    std::cout << "🧪 Testing clear() memory release..." << std::endl;
    
    List<int> lst{resource.get()};
    
    // Заполняем список
    for (int i = 0; i < 10; ++i) {
        lst.push_front(i);
    }

    size_t allocated_before_clear = resource->allocated_count();
    std::cout << "   Before clear: " << allocated_before_clear << " blocks allocated" << std::endl;
    EXPECT_GT(allocated_before_clear, 0);

    lst.clear();
    
    EXPECT_TRUE(lst.empty());
    EXPECT_EQ(lst.size(), 0);
    EXPECT_EQ(resource->allocated_count(), 0);
    EXPECT_GT(resource->free_count(), 0);

    std::cout << "   After clear: " << resource->allocated_count() << " allocated, " 
              << resource->free_count() << " cached" << std::endl;
}

TEST_F(ListPMRTest, StringList) {
    std::cout << "🧪 Testing List with strings..." << std::endl;
    
    List<std::string> str_lst{resource.get()};
    
    str_lst.push_front("world");
    str_lst.push_front("hello");
    str_lst.push_front("test");

    EXPECT_EQ(str_lst.front(), "test");
    
    auto it = str_lst.begin();
    EXPECT_EQ(*it, "test");
    ++it;
    EXPECT_EQ(*it, "hello");
    ++it;
    EXPECT_EQ(*it, "world");

    std::cout << "   String list works correctly" << std::endl;
}

TEST_F(ListPMRTest, MemoryReuse) {
    std::cout << "🧪 Testing memory reuse..." << std::endl;
    
    size_t initial_cached = resource->free_count();
    std::cout << "   Initially cached: " << initial_cached << " blocks" << std::endl;

    {
        List<int> lst1{resource.get()};
        lst1.push_front(1);
        lst1.push_front(2);
        lst1.push_front(3);
    }

    size_t cached_after_first_list = resource->free_count();
    std::cout << "   After first list: " << cached_after_first_list << " cached blocks" << std::endl;

    {
        List<int> lst2{resource.get()};
        lst2.push_front(4);
        lst2.push_front(5);
        lst2.push_front(6);
    }

    // Должны переиспользовать кешированные блоки
    size_t cached_after_second_list = resource->free_count();
    std::cout << "   After second list: " << cached_after_second_list << " cached blocks" << std::endl;

    EXPECT_GE(cached_after_second_list, cached_after_first_list);
}

TEST_F(ListPMRTest, LargeList) {
    std::cout << "🧪 Testing large list..." << std::endl;
    
    const int NUM_ELEMENTS = 100;
    List<int> lst{resource.get()};

    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        lst.push_front(i);
    }

    EXPECT_EQ(lst.size(), NUM_ELEMENTS);
    EXPECT_EQ(resource->allocated_count(), NUM_ELEMENTS);

    int count = 0;
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        ++count;
    }
    EXPECT_EQ(count, NUM_ELEMENTS);

    std::cout << "   Large list with " << NUM_ELEMENTS << " elements works correctly" << std::endl;
    std::cout << "   Memory blocks used: " << resource->allocated_count() << std::endl;
}