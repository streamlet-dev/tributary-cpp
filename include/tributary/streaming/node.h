#pragma once
#include <any>
#include <functional>
#include <iostream>
#include <string>
#include <tributary/base.h>
#include <tributary/utils.h>

namespace tributary {
namespace streaming {

using namespace tributary::utils;

// auto _defaultFunction = [&]() { return 1; }
static auto _defaultFunction = []() { return 1; };

// https://stackoverflow.com/questions/57037888/how-to-store-any-kind-of-function-in-a-variable-in-c
template <typename Function, typename... Args>
class T_EXPORT Node {
public:
  Node(Function _function)
    : function(_function)
    , name("Node")
    , id(generateUUID()) {
      std::cout << *this << std::endl;
    }

  auto
  operator()(Args... _args) {
    // std::function<std::invoke_result_t<Function, Args...>()> mappedFunction = dynamic_cast<std::function<std::invoke_result_t<Function,
    // Args...>()>>(function);
    return function(std::forward<Args>(_args)...);
  }
  // auto getName() const {return name;}
  // auto getId() const {return id;}


  friend T_EXPORT std::ostream& operator<<(std::ostream& ostream, const Node<Function, Args...>& node) {
    // ostream << node.getName() << "[" << node.getId().substr(0, 6) << "]";
    ostream << node.name << "[" << node.id.substr(0, 6) << "]";
    return ostream;
  }

protected:
  std::string name;
  std::string id;
  std::function<std::invoke_result_t<Function, Args...>(Args...)> function;
  // std::function<std::invoke_result_t<Function, Args...>()> function;
};


template <typename Function, typename... Args>
class T_EXPORT Foo : public Node<Function, Args...> {
public:
  template <typename... Ts>
  Foo(Function _function, std::tuple<Ts...> functionArgs)
    : Node<Function, Args...>(std::bind(_function, functionArgs)) {}

protected:
};

} // namespace streaming
} // namespace tributary
