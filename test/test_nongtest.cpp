#ifdef WIN32
#pragma comment(lib, "tributary")
#endif

#include <iostream>
#include <cppcoro/sync_wait.hpp>
#include <tributary/streaming.h>
#include <tributary/utils.h>

using namespace tributary::streaming;
using namespace tributary::utils;

int func() { return 1;} 

int main() {
    std::cout << "Export check: " << streamingExportCheck() << std::endl;

    std::cout << "*******************************************" << std::endl;

    Node n(generator);
    // std::cout << n << std::endl;
    std::cout << n() << std::endl;
    std::cout << n() << std::endl;
    std::cout << n() << std::endl;
    std::cout << n() << std::endl;

    std::cout << "*******************************************" << std::endl;

    std::cout << cppcoro::sync_wait(asyncFunctionCoro()) << std::endl;
    std::cout << cppcoro::sync_wait(asyncFunctionCoro()) << std::endl;
    std::cout << cppcoro::sync_wait(asyncFunctionCoro()) << std::endl;
    std::cout << cppcoro::sync_wait(asyncFunctionCoro()) << std::endl;

    std::cout << "*******************************************" << std::endl;

    // std::function<cppcoro::task<int>()> foo = tributary::streaming::convertToCoroutine<std::function<int()>>(func);
    std::function<cppcoro::task<int>()> foo = tributary::streaming::convertToCoroutine(generator);
    std::cout << cppcoro::sync_wait(foo()) << std::endl;
    std::cout << cppcoro::sync_wait(foo()) << std::endl;
    std::cout << cppcoro::sync_wait(foo()) << std::endl;
    std::cout << cppcoro::sync_wait(foo()) << std::endl;

    std::cout << "*******************************************" << std::endl;


}
