#pragma once
#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <tuple>
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

class T_EXPORT BaseNode {
    friend T_EXPORT std::ostream& operator<<(std::ostream& ostream, const BaseNode& node) {
        return (ostream << node.name);
    }

protected:
  BaseNode() = default;
  const std::string name = "basenode"; 
};

class T_EXPORT StreamNone : public BaseNode {
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

class T_EXPORT StreamBegin : public BaseNode {
public:
  static StreamBegin& inst() {
    static StreamBegin inst;
    return inst;
  }

private:
  StreamBegin() = default;
  ~StreamBegin() = default;
  const std::string name = "StreamBegin"; 
};

class T_EXPORT StreamEnd : public BaseNode {
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

    t_value(BaseNode& _state)
    : hasValue(false)
    , state(&_state) {}

    t_value(BaseNode* _state)
    : hasValue(false)
    , state(_state) {}

    t_value(T _value) 
    : hasValue(true)
    , state(NULL)
    , value(_value) {}

    template <typename T>
    bool isStreamType() const {
      if (hasValue) return false;
      return state == &(T::inst());
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
    BaseNode* state;
    T value;
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
