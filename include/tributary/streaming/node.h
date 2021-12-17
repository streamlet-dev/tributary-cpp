#pragma once
#include <any>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <tributary/base.h>
#include <tributary/utils.h>

namespace tributary {
namespace streaming {

using namespace tributary::utils;

// auto _defaultFunction = [&]() { return 1; }
static auto _defaultFunction = []() { return 1; };

// https://stackoverflow.com/questions/57037888/how-to-store-any-kind-of-function-in-a-variable-in-c
template <typename Function, typename... Args>
class T_EXPORT Node : public BaseNode {
public:
  Node(Function _function)
    : name("Node")
    , id(generateUUID())
    , function(_function) {}

  auto
  operator()(Args... _args) {
    // std::function<std::invoke_result_t<Function, Args...>()> mappedFunction = dynamic_cast<std::function<std::invoke_result_t<Function,
    // Args...>()>>(function);
    return function(std::forward<Args>(_args)...);
  }


//  def value(self):
//         """get value from node"""
//         return self._last

  // async def __call__(self):
  //     """execute the callable if possible, and propogate values downstream"""
  //     # Downstream nodes can't process
  //     if self._backpressure():
  //         return StreamNone()

  //     # Previously ended stream
  //     if self._finished:
  //         return await self._finish()

  //     # Sleep if needed
  //     if self._delay_interval:
  //         await asyncio.sleep(self._delay_interval)

  //     # Stop executing
  //     if self._execution_max > 0 and self._execution_count >= self._execution_max:
  //         self._func = lambda: StreamEnd()
  //         self._old_func = lambda: StreamEnd()

  //     ready = True
  //     # iterate through inputs
  //     for i, inp in enumerate(self._input):
  //         # if input hasn't received value
  //         if isinstance(self._active[i], StreamNone):
  //             if len(inp) > 0:
  //                 # get from input queue
  //                 val = inp.popleft()

  //                 while isinstance(val, StreamRepeat):
  //                     # Skip entry
  //                     val = inp.popleft()

  //                 if isinstance(val, StreamEnd):
  //                     return await self._finish()

  //                 # set as active
  //                 self._active[i] = val
  //             else:
  //                 # wait for value
  //                 self._active[i] = StreamNone()
  //                 ready = False


  friend T_EXPORT std::ostream& operator<<(std::ostream& ostream, const Node<Function, Args...>& node) {
    // ostream << node.getName() << "[" << node.getId().substr(0, 6) << "]";
    ostream << node.name << "[" << node.id.substr(0, 6) << "]";
    return ostream;
  }

protected:
  // Name of the node
  std::string name;

  // Unique ID of the node
  std::string id;

  // Construct return type of provided function
  using ReturnType = std::invoke_result_t<Function, Args...>;

  // Stored function
  std::function<ReturnType(Args...)> function;

  // Last value of function
  // if last.first is not NULL, it is the result, otherwise return last.second
  std::tuple<BaseNode*, ReturnType> last;

private:
  bool _backpressure() const;

  static constexpr std::size_t Inputs = sizeof...(Args);
  Node* upstream[Inputs];
  Node* downstream[Inputs];
  // std::function<std::invoke_result_t<Function, Args...>()> function;
};


template <typename Function, typename... Args>
class T_EXPORT Foo : public Node<Function, Args...> {
public:
  template <typename... Ts>
  Foo(Function _function, std::tuple<Ts...> functionArgs)
    : Node<Function, Args...>(std::bind(_function, functionArgs)) {}

protected:
};

} // namespace streaming
} // namespace tributary
