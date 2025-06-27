#pragma once
#include <iostream>
#include <tributary/base.hpp>
#include <tributary/types.hpp>
#include <tributary/utils.hpp>

using namespace std;
using namespace tributary::utils;

namespace tributary {
namespace lazy {

template <typename Function, typename... Args>
class T_EXPORT Node {
public:
  Node(Function _function)
    : func(_function)
    , name("Node")
    , id(generateUUID()) {}

  auto
  operator()(Args... _args) {
    return func(forward<Args>(_args)...);
  }

  friend T_EXPORT std::ostream& operator<<(std::ostream& ostream, const Node<Function, Args...>& node) {
    ostream << node.name << "[" << node.id.substr(0, 6) << "]";
    return ostream;
  }

protected:
  t_str name;
  t_str id;
  t_func<invoke_result_t<Function, Args...>(Args...)> func;
};


} // namespace lazy
} // namespace tributary
