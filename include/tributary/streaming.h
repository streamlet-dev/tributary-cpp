#pragma once
#include <functional>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <tributary/base.h>

namespace tributary {
namespace streaming {

template <typename Ret, typename... Args>
class Node {
  typedef std::function<Ret(Args&...)> FuncArg;

public:
  Node(FuncArg& func)
    : func(func)
    , name("Node")
    , id(Node::generateUUID()) {}

  Node()
    : func([&]() { return 1; })
    , name("Node")
    , id(Node::generateUUID()) {}

  int
  operator()() {
    return func();
  }
  static std::string
  generateUUID() {
    static boost::uuids::random_generator generator;
    std::string uuid = boost::uuids::to_string(generator());
    boost::replace_all(uuid, "-", "");
    return uuid;
  };

  friend std::ostream&
  operator<<(std::ostream& ostream, const Node<Ret, Args&...>& node) {
    ostream << node.name << "[" << node.id.substr(0, 6) << "]";
    return ostream;
  }

private:
  std::string name;
  std::string id;
  FuncArg func;
};

} // namespace streaming
} // namespace tributary
