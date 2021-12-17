#pragma once
#include <functional>
#include <string>
#include <tributary/streaming/node.h>

namespace tributary {
namespace streaming {
namespace output {

template <typename T>
class T_EXPORT Print : public Node<std::function<T(T)>, T> {
public:
  Print(std::string _text = "")
    : Node<std::function<T(T)>, T>([this](T value) -> T {
      std::cout << this->text << value << std::endl;
      return value;
    })
    , text(_text) {
    this->name = "Print";
  }

private:
  std::string text;
};

} // namespace output
} // namespace streaming
} // namespace tributary
