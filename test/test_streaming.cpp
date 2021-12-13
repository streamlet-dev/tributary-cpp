#include <iostream>
#include <gtest/gtest.h>
#include <tributary/streaming.h>

using namespace tributary::streaming;

TEST(StreamingTest, Basics) {
    EXPECT_NO_THROW({
        Node n;
        std::cout << n << std::endl;

    });
}
