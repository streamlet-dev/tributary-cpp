#pragma once
#include <tributary/types.hpp>
#include <tributary/streaming/node.hpp>

using namespace std;

namespace tributary {
namespace streaming {
namespace output {

template <typename T>
class T_EXPORT Print : public Node<t_func<T(T)>, T> {
public:
  Print(t_str _text = "")
    : Node<t_func<T(T)>, T>([this](T value) -> T {
      cout << this->text << value << endl;
      return value;
    })
    , text(_text) {
    this->name = "Print";
  }

private:
  t_str text;
};

} // namespace output
} // namespace streaming
} // namespace tributary
