#ifdef WIN32
#pragma comment(lib, "tributary")
#endif

#include <iostream>
#include <cppcoro/sync_wait.hpp>
#include <tributary/streaming.hpp>
#include <tributary/types.hpp>
#include <tributary/utils.hpp>

using namespace std;
using namespace tributary;
using namespace tributary::streaming;
using namespace tributary::utils;

int func() { return 1;} 

void hr() {
    cout << "*******************************************" << endl;
}

void basics() {
    Node n(generator);
    // cout << n << endl;
    cout << n() << endl;
    cout << n() << endl;
    cout << n() << endl;
    cout << n() << endl;

    hr();

    cout << cppcoro::sync_wait(asyncFunctionCoro()) << endl;
    cout << cppcoro::sync_wait(asyncFunctionCoro()) << endl;
    cout << cppcoro::sync_wait(asyncFunctionCoro()) << endl;
    cout << cppcoro::sync_wait(asyncFunctionCoro()) << endl;

    hr();

    // function<cppcoro::task<int>()> foo = tributary::streaming::convertToCoroutine<function<int()>>(func);
    function<t_fut<int>()> foo = tributary::streaming::convertToCoroutine(generator);
    cout << cppcoro::sync_wait(foo()) << endl;
    cout << cppcoro::sync_wait(foo()) << endl;
    cout << cppcoro::sync_wait(foo()) << endl;
    cout << cppcoro::sync_wait(foo()) << endl;

    hr();
}

void stream() {
    tributary::streaming::Node n(generator);
    tributary::streaming::output::Print<int> p("out:");

    n.pipe(p);
    // n >> p;

    n();
    
    cout << p().value << endl;
    // auto add = n + c1 + 3;
    // auto output = tributary::streaming::output::Print(add);
    // tributary::streaming::run(output);
}

// void stream() {
//     tributary::streaming::Node n(generator);
//     tributary::streaming::input::Const c1(5);


//     // auto add = n + c1 + 3;
//     // auto output = tributary::streaming::output::Print(add);
//     // tributary::streaming::run(output);
// }

int main() {
    cout << "Export check: " << streamingExportCheck() << endl;
    hr();
    basics();
    hr();
    stream();
}
