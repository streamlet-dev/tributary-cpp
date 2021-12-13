#pragma once
#include <functional>
#include <iostream>

namespace tributary {
namespace streaming {

class Node {
public:
  Node()
    : func([&](int var) { return var; })
    , name("Node")
    , id(Node::generateUUID()) {}

  static std::string generateUUID();

  friend std::ostream& operator<<(std::ostream& ostream, const Node& node);

private:
  std::string name;
  std::string id;
  std::function<int(int)> func;
};

} // namespace streaming
} // namespace tributary
