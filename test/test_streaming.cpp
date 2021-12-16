#include <iostream>
#include <gtest/gtest.h>
#include <tributary/streaming.h>
#include <tributary/utils.h>

using namespace tributary::streaming;
using namespace tributary::utils;

TEST(StreamingTest, Instantiation) {
    // EXPECT_NO_THROW({
    //     Node n;
    //     std::cout << n << std::endl;
    // });

    EXPECT_NO_THROW({
        Node n(generator);
        std::cout << n << std::endl;
    });

    EXPECT_NO_THROW({
        Node n(addTo(5));
        std::cout << n << std::endl;
    });
}

TEST(StreamingTest, Generator) {
    Node n(generator);
    EXPECT_EQ(n(), 0);
    EXPECT_EQ(n(), 1);
    EXPECT_EQ(n(), 2);
    EXPECT_EQ(n(), 3);

    Node n2(addTo(5));
    EXPECT_EQ(n2(), 5);
    EXPECT_EQ(n2(), 6);
    EXPECT_EQ(n2(), 7);
    EXPECT_EQ(n2(), 8);
}

TEST(StreamingTest, Const) {
    input::Const c(5);
    EXPECT_EQ(c(), 5);
    EXPECT_EQ(c(), 5);
}


TEST(StreamingTest, Print) {
    output::Print<int> p("output:");
    EXPECT_EQ(p(5), 5);
}
