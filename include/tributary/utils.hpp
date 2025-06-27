#pragma once

#include <functional>
#include <future>
#include <cppcoro/task.hpp>
#include <cppcoro/generator.hpp>
#include <tributary/base.hpp>
#include <tributary/types.hpp>

using namespace std;

namespace tributary {
namespace utils {

// https://stackoverflow.com/questions/9059187/equivalent-c-to-python-generator-pattern
static t_func<int()> generator = [] {
  int i = 0;
  return [=]() mutable { return i < 10 ? i++ : -1; };
}();

static t_func<t_sfut<int>()> asyncFunction = [] {
  int i = 0;
  return [=]() mutable { return async(launch::async, [=]() mutable -> int { return i < 10 ? i++ : -1; }); };
}();

T_EXPORT t_fut<int> asyncFunctionCoro();

T_EXPORT t_gen<int> asyncGenerator();

T_EXPORT t_func<int()> addTo(int x);

T_EXPORT t_str generateUUID();

} // namespace utils
} // namespace tributary
