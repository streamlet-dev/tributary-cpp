#pragma once
#include <cppcoro/task.hpp>
#include <tributary/base.h>
#include <tributary/streaming/node.h>
#include <tributary/streaming/input.h>
#include <tributary/streaming/output.h>

namespace tributary {
namespace streaming {

T_EXPORT int streamingExportCheck();

// template <typename Function, typename... Args>
// T_EXPORT std::function<cppcoro::task<std::invoke_result_t<Function, Args...> >(Args...)> convertToCoroutine(
//   std::function<std::invoke_result_t<Function, Args...>(Args...)> function);

template <typename Function>
inline T_EXPORT std::function<cppcoro::task<std::invoke_result_t<Function>>()>
convertToCoroutine(Function function) {
  auto func = [function = function]() -> cppcoro::task<std::invoke_result_t<Function>> { co_return function(); };
  return func;
}

template <typename Function>
inline T_EXPORT cppcoro::task<std::invoke_result_t<Function>>
convertToCoroutine(cppcoro::task<std::invoke_result_t<Function>> coro) {
  return coro;
};

} // namespace streaming
} // namespace tributary
