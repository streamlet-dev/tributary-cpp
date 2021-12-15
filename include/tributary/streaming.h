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

  Node()
    : function(_defaultFunction)
    , name("Node")
    , id(generateUUID()) {}

  Node(Function _function, Args... _args )
    : function(std::bind(_function, std::forward<Args>(_args)...))
    , name("Node")
    , id(generateUUID()) {}

  auto operator()() { return function(); }
  // auto getName() const {return name;}
  // auto getId() const {return id;}

  friend T_EXPORT std::ostream& operator<<(std::ostream& ostream, const Node<Function, Args...>& node) {
    // ostream << node.getName() << "[" << node.getId().substr(0, 6) << "]";
    ostream << node.name << "[" << node.id.substr(0, 6) << "]";
    return ostream;
  }


private:
  std::string name;
  std::string id;
  using ReturnType = std::invoke_result_t<Function, Args...>;
  std::function<ReturnType()> function;
};

} // namespace streaming
} // namespace tributary
