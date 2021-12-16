#ifdef WIN32
#pragma comment(lib, "tributary")
#endif

#include <iostream>
#include <cppcoro/sync_wait.hpp>
#include <tributary/streaming.h>
#include <tributary/utils.h>

using namespace tributary::streaming;
using namespace tributary::utils;

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

    std::cout << cppcoro::sync_wait(asyncGeneratorCoro()) << std::endl;
    std::cout << cppcoro::sync_wait(asyncGeneratorCoro()) << std::endl;
    std::cout << cppcoro::sync_wait(asyncGeneratorCoro()) << std::endl;
    std::cout << cppcoro::sync_wait(asyncGeneratorCoro()) << std::endl;

    std::cout << "*******************************************" << std::endl;


}
