#include <iostream>
#include <gtest/gtest.h>
#include <tributary/streaming.h>

using namespace tributary::streaming;

// https://stackoverflow.com/questions/9059187/equivalent-c-to-python-generator-pattern
std::function<int()> generator = []{
  int i = 0;
  return [=]() mutable {
    return i < 10 ? i++ : -1;
  };
}();

TEST(StreamingTest, Instantiation) {
    EXPECT_NO_THROW({
        Node n;
        std::cout << n << std::endl;
    });

    EXPECT_NO_THROW({
        Node n(generator);
        std::cout << n << std::endl;
    });
}

TEST(StreamingTest, Generator) {
    Node n(generator);
    EXPECT_EQ(n(), 0);
    EXPECT_EQ(n(), 1);
    EXPECT_EQ(n(), 2);
    EXPECT_EQ(n(), 3);
}
