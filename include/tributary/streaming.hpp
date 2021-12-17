#pragma once
#include <tributary/base.hpp>
#include <tributary/types.hpp>
#include <tributary/streaming/node.hpp>
#include <tributary/streaming/input.hpp>
#include <tributary/streaming/output.hpp>

using namespace std;

namespace tributary {
namespace streaming {

T_EXPORT int streamingExportCheck();

// template <typename Function, typename... Args>
// T_EXPORT std::function<cppcoro::task<std::invoke_result_t<Function, Args...> >(Args...)> convertToCoroutine(
//   std::function<std::invoke_result_t<Function, Args...>(Args...)> function);

template <typename Function>
inline T_EXPORT t_func<t_fut<invoke_result_t<Function>>()> convertToCoroutine(Function _func) {
  auto ret = [func = _func]() -> t_fut<invoke_result_t<Function>> { co_return func(); };
  return ret;
}

template <typename Function>
inline T_EXPORT t_fut<invoke_result_t<Function>> convertToCoroutine(t_fut<invoke_result_t<Function>> coro) {
  return coro;
};

} // namespace streaming
} // namespace tributary
