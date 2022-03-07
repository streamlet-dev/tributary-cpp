#pragma once
#include <any>
#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <boost/algorithm/string/replace.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cppcoro/generator.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <tributary/base.hpp>

namespace tributary {

class T_EXPORT BaseValue {
  friend T_EXPORT std::ostream& operator<<(std::ostream& ostream, const BaseValue& value) { return (ostream << value.name); }

protected:
  BaseValue() = default;
  const std::string name = "BaseValue";

};

class T_EXPORT StreamNone : public BaseValue {
public:
  static StreamNone& inst() {
    static StreamNone inst;
    return inst;
  }

private:
  StreamNone() = default;
  ~StreamNone() = default;
  const std::string name = "StreamNone";
};

class T_EXPORT StreamRepeat : public BaseValue {
public:
  static StreamRepeat& inst() {
    static StreamRepeat inst;
    return inst;
  }

private:
  StreamRepeat() = default;
  ~StreamRepeat() = default;
  const std::string name = "StreamRepeat";
};

class T_EXPORT StreamEnd : public BaseValue {
public:
  static StreamEnd& inst() {
    static StreamEnd inst;
    return inst;
  }

private:
  StreamEnd() = default;
  ~StreamEnd() = default;
  const std::string name = "StreamEnd";
};

// Last value of function
// if last.first is not NULL, it is the result, otherwise return last.second
template <typename T>
struct T_EXPORT t_value {
  t_value()
    : hasValue(false)
    , state(&StreamNone::inst()) {}

  t_value(BaseValue& _state)
    : hasValue(false)
    , state(&_state) {}

  t_value(BaseValue* _state)
    : hasValue(false)
    , state(_state) {}

  t_value(T _value)
    : hasValue(true)
    , state(NULL)
    , value(_value) {}

  template <typename U>
  bool isStreamType() const {
    if (hasValue)
      return false;
    return state == &(U::inst());
  }

  bool isStreamNone() const { return isStreamType<StreamNone>(); }
  bool isStreamRepeat() const { return isStreamType<StreamRepeat>(); }
  bool isStreamEnd() const { return isStreamType<StreamEnd>(); }

  friend T_EXPORT std::ostream& operator<<(std::ostream& ostream, const t_value<T>& value) {
    if (value.hasValue) {
      ostream << value.value;
    } else {
      ostream << *(value.state);
    }
    return ostream;
  }

  bool hasValue;
  BaseValue* state;
  T value;
};


class T_EXPORT BaseNode {
  friend T_EXPORT std::ostream& operator<<(std::ostream& ostream, const BaseNode& node) { return (ostream << node.name); }
protected:
  BaseNode() = default;
  const std::string name = "Base";
};


typedef std::string t_str;
typedef tributary::BaseNode t_node;
typedef std::shared_ptr<t_node> t_nodeptr;

template <typename T>
using t_sfut = std::future<T>;

template <typename T>
using t_func = std::function<T>;

template <typename T>
using t_fut = cppcoro::task<T>;

template <typename T>
using t_gen = cppcoro::generator<T>;

} // namespace tributary
