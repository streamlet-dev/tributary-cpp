#pragma once
#include <tributary/types.hpp>
#include <tributary/streaming/node.hpp>

using namespace std;

namespace tributary {
namespace streaming {
namespace input {

template <typename T>
class T_EXPORT Const : public Node<t_func<T()>> {
public:
  Const(T _value)
    : Node<t_func<T()>>([&]() { return value; })
    , value(_value) {
    this->name = "Const";
  }

private:
  T value;
};

} // namespace input
} // namespace streaming
} // namespace tributary
