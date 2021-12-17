#pragma once
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
#include <cppcoro/task.hpp>
#include <cppcoro/generator.hpp>
#include <tributary/base.hpp>

namespace tributary {

class T_EXPORT BaseNode {
protected:
  BaseNode() = default;
};

class T_EXPORT StreamNone : public BaseNode {
public:
  StreamNone& inst() {
    static StreamNone inst;
    return inst;
  }

private:
  StreamNone() = default;
  ~StreamNone() = default;
};

class T_EXPORT StreamBegin : public BaseNode {
public:
  StreamBegin& inst() {
    static StreamBegin inst;
    return inst;
  }

private:
  StreamBegin() = default;
  ~StreamBegin() = default;
};

class T_EXPORT StreamEnd : public BaseNode {
public:
  StreamEnd& inst() {
    static StreamEnd inst;
    return inst;
  }

private:
  StreamEnd() = default;
  ~StreamEnd() = default;
};

} // namespace tributary

typedef std::string t_str;
typedef tributary::BaseNode t_node;
typedef std::vector<std::shared_ptr<t_node>> t_nodelist;

// Last value of function
// if last.first is not NULL, it is the result, otherwise return last.second
template <typename T>
using t_value = std::tuple<std::shared_ptr<t_node>, T>;

template <typename T>
using t_sfut = std::future<T>;

template <typename T>
using t_func = std::function<T>;

template <typename T>
using t_fut = cppcoro::task<T>;

template <typename T>
using t_gen = cppcoro::generator<T>;
