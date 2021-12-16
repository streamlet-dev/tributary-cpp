#pragma once
#include <any>
#include <functional>
#include <iostream>
#include <tributary/base.h>
#include <tributary/utils.h>

namespace tributary {
namespace streaming {

using namespace tributary::utils;

// auto _defaultFunction = [&]() { return 1; }
static auto _defaultFunction = []() { return 1; };

// https://stackoverflow.com/questions/57037888/how-to-store-any-kind-of-function-in-a-variable-in-c
template <typename Function, typename... Args>
class Node {
public:
  Node(Function _function, Args... _args)
    : function(std::bind(_function, std::forward<Args>(_args)...))
    , name("Node")
    , id(generateUUID()) {}

  auto
  operator()() {
    return function();
  }
  // auto getName() const {return name;}
  // auto getId() const {return id;}

  friend T_EXPORT std::ostream&
  operator<<(std::ostream& ostream, const Node<Function, Args...>& node) {
    // ostream << node.getName() << "[" << node.getId().substr(0, 6) << "]";
    ostream << node.name << "[" << node.id.substr(0, 6) << "]";
    return ostream;
  }

protected:
  std::string name;
  std::string id;
  std::function<std::invoke_result_t<Function, Args...>()> function;
};

namespace input {

template <typename T>
class Const : public Node<std::function<T()>> {
public:
  Const(T _value)
    : Node<std::function<T()>>([&]() { return value; })
    , value(_value) {
    this->name = "Const";
  }

private:
  T value;
};

} // namespace input

namespace output {

// template <typename T>
// class Print : public Node<std::function<T(T)>> {
// public:
//   Print(std::string _text = "")
//     : Node<std::function<T(T)>>([&](T value) -> T {
//       std::cout << text << value << std::endl;
//       return value;
//     })
//     , text(_text) {
//     this->name = "Print";
//   }

//   T operator()(T input) {
//     return function(input);
//   }

// private:
//   std::string text;
// };



// class Print : public Node<std::function<int(int)>, int> {
// public:
//   Print(std::string _text = "")
//     : Node<std::function<int(int)>, int>([&](int value) -> int {
//       std::cout << text << value << std::endl;
//       return value;
//     }, 5)
//     , text(_text) {
//     this->name = "Print";
//   }

//   // int operator()(int input) {
//   //   return function(input);
//   // }

// private:
//   std::string text;
// };

} // namespace output

} // namespace streaming
} // namespace tributary
