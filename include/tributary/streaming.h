#pragma once
#include <functional>
#include <iostream>

namespace tributary {
namespace streaming {

typedef std::function<int(void)> FuncArg;

class Node {
public:
  Node(FuncArg& func)
    : func(func)
    , name("Node")
    , id(Node::generateUUID()) {}

  Node()
    : func([&]() { return 1; })
    , name("Node")
    , id(Node::generateUUID()) {}

  int operator()() { return func();}
  static std::string generateUUID();

  friend std::ostream& operator<<(std::ostream& ostream, const Node& node);

private:
  std::string name;
  std::string id;
  FuncArg func;
};

} // namespace streaming
} // namespace tributary
