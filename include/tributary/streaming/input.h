#pragma once
#include <functional>
#include <tributary/streaming/node.h>

namespace tributary {
namespace streaming {
namespace input {

template <typename T>
class T_EXPORT Const : public Node<std::function<T()>> {
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
} // namespace streaming
} // namespace tributary
