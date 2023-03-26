#include <MemPool.h>
#include <gtest/gtest.h>

struct Item {
    uint8_t buffer[256];
};

TEST(MemoryPool, Allocate) {
    MemPool<Item, 5> pool;
    ASSERT_TRUE(pool.alloc() != nullptr);
    ASSERT_TRUE(pool.alloc() != nullptr);
    ASSERT_TRUE(pool.alloc() != nullptr);
    ASSERT_TRUE(pool.alloc() != nullptr);
    ASSERT_TRUE(pool.alloc() != nullptr);
    ASSERT_TRUE(pool.alloc() == nullptr);
}

TEST(MemoryPool, AllocateAndFreeSequently) {
    MemPool<Item, 5> pool;
    for (int i = 0; i < pool.size() * 20; i++) {
        Item *item = pool.alloc();
        ASSERT_TRUE(item != nullptr);
        ASSERT_EQ(pool.free(item), Res::Ok);
    }
}

TEST(MemoryPool, AllocateAllThenFreeAll) {
    MemPool<Item, 5> pool;
    Item *pointers[5];
    for (int k = 0; k < 5; k++) {
        for (int i = 0; i < pool.size(); i++) {
            pointers[i] = pool.alloc();
            ASSERT_TRUE(pointers[i] != nullptr);
        }
        for (int i = 0; i < pool.size(); i++) {
            for (int j = i + 1; j < pool.size(); j++) {
                ASSERT_NE(pointers[i], pointers[j]);
            }
        }
        for (int i = 0; i < pool.size(); i++) {
            ASSERT_EQ(pool.free(pointers[i]), Res::Ok);
        }
    }
}

TEST(MemoryPool, DoubleFree) {
    MemPool<Item, 5> pool;
    auto ptr = pool.alloc();
    ASSERT_TRUE(ptr != nullptr);
    ASSERT_EQ(pool.free(ptr), Res::Ok);
    ASSERT_EQ(pool.free(ptr), Res::Error);
}

TEST(MemoryPool, BadFree) {
    MemPool<Item, 5> pool;
    Item *ptr = pool.alloc();
    ASSERT_TRUE(ptr != nullptr);
    ptr++;
    ASSERT_EQ(pool.free(ptr), Res::Error);
    ptr = (Item*)(((uint8_t*)ptr)[10]);
    ASSERT_EQ(pool.free(ptr), Res::InvalidPtr);
}

TEST(MemoryPool, DataFill) {
    MemPool<Item, 5> pool;

    Item *pointers[5];
    for (int i = 0; i < pool.size(); i++) {
        pointers[i] = pool.alloc();
        memset(pointers[i]->buffer, 0x11 * (i + 1), sizeof(Item::buffer));
    }

    for (int i = 0; i < pool.size(); i++) {
        Item testData;
        memset(testData.buffer, 0x11 * (i + 1), sizeof(Item::buffer));
        ASSERT_TRUE(memcmp(pointers[i]->buffer, testData.buffer, sizeof(Item::buffer)) == 0);
    }
}
