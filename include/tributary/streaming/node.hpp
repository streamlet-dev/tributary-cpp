#pragma once
#include <chrono>
#include <deque>
#include <iostream>
#include <thread>
#include <vector>
#include <tributary/base.hpp>
#include <tributary/utils.hpp>

using namespace tributary::utils;
using namespace std;

namespace tributary {
namespace streaming {

// auto _defaultFunction = [&]() { return 1; }
static auto _defaultFunction = []() { return 1; };

// https://stackoverflow.com/questions/57037888/how-to-store-any-kind-of-function-in-a-variable-in-c
template <typename Function, typename... Args>
class T_EXPORT Node : public BaseNode {
  // Construct return type of provided function
  using ReturnType = invoke_result_t<Function, Args...>;
  using ValueType = t_value<ReturnType>;

public:
  Node(Function _func)
    : name("Node")
    , id(generateUUID())
    , func(_func) {}

  auto value() {
    return last;
  }

  ValueType
  operator()(Args... _args) {
    // Downstream nodes can't process
    if (_backpressure()) {
      return StreamNone::inst();
    }

    // Previously ended stream
    if (_finished) {
      return _finish();
    }

    // Sleep if needed
    if (_delay_interval > 0ns) {
      this_thread::sleep_for(_delay_interval);
    }

    // Stop executing
    if (_execution_max > 0 && _execution_count >= _execution_max) {
      // self._func = lambda: StreamEnd()
      // self._old_func = lambda: StreamEnd()
    }

    bool ready = true;
    // iterate through inputs 
    for (auto i = 0; i < _input.size(); ++i ) {
      // if input hasn't received value
      if (_active[i].isStreamNone()) {
        auto inp = _input[i];
        if (inp.size() > 0) {
          // get from input queue
          auto val = inp.front();
          inp.pop_front();

          while (val.isStreamRepeat()) {
            // skip entry
            val = inp.front();
            inp.pop_front();
          }

          if (val.isStreamEnd()) {
            return _finish();
          }

          // set as active
          _active[i] = val;

        } else {
          // wait for value
          //         self._active[i] = StreamNone()
          //         ready = False
        }
      }
    }

    if (ready) {
      // execute function
      // return await self._execute()
    }

    return func(forward<Args>(_args)...);
  }

  Node& operator>>(Node& rhs) {
    // wire lhs into rhs
    _downstream.push_back(t_nodepos(&rhs, rhs._upstream.size()));
    rhs._upstream.push_back(this);
  }

  Node& operator<<(Node& rhs) {
    // wire rhs into lhs
    rhs._downstream.push_back(t_nodepos(this, _upstream.size()));
    _upstream.push_back(&rhs);
  }

  void _push(ValueType& inp, size_t index) {
    // push value to downstream nodes
    _input[index].push_back(inp);
  }

  bool _empty(size_t index) {
    // check if value
    return _input[index].size() == 0 || _active[index].isNone();
  }

  // async def _pop(self, index):
  //     """pop value from downstream nodes"""
  //     if len(self._input[index]) > 0:
  //         return self._input[index].popleft()



  // async def _execute(self):
  //     """execute callable"""
  //     # assume no valid input
  //     valid = False

  //     # wait for valid input
  //     while not valid:
  //         # await if its a coroutine
  //         if asyncio.iscoroutine(self._func):
  //             _last = await self._func(*self._active, **self._func_kwargs)

  //         # else call it
  //         elif isinstance(self._func, types.FunctionType):
  //             try:
  //                 # could be a generator
  //                 try:
  //                     _last = self._func(*self._active, **self._func_kwargs)
  //                 except ZeroDivisionError:
  //                     _last = float("inf")

  //             except ValueError:
  //                 # Swap back to function to get a new generator next iteration
  //                 self._func = self._old_func
  //                 continue

  //         else:
  //             raise TributaryException("Cannot use type:{}".format(type(self._func)))

  //         # calculation was valid
  //         valid = True

  //         # increment execution count
  //         self._execution_count += 1

  //     if isinstance(_last, types.AsyncGeneratorType):

  //         async def _func(g=_last):
  //             return await _agen_to_func(g)

  //         self._func = _func
  //         _last = await self._func()

  //     elif isinstance(_last, types.GeneratorType):
  //         # Swap to generator unroller
  //         self._old_func = self._func
  //         self._func = lambda g=_last: _gen_to_func(g)
  //         _last = self._func()

  //     elif asyncio.iscoroutine(_last):
  //         _last = await _last

  //     if self._repeat:
  //         if isinstance(_last, (StreamNone, StreamRepeat)):
  //             # NOOP
  //             self._last = self._last
  //         else:
  //             self._last = _last
  //     else:
  //         self._last = _last

  //     await self._output(self._last)

  //     for i in range(len(self._active)):
  //         self._active[i] = StreamNone()

  //     await self._enddd3g()
  //     if isinstance(self._last, StreamEnd):
  //         await self._finish()

  ValueType
  _finish() {
    // mark this node as finished
    _finished = true;
    _last = StreamEnd::inst();
    return _output(_last);
  }


  ValueType
  _output(ValueType ret) {
    // output value to downstream nodes
    // if downstreams, output
    if (!ret.isStreamNone() && !ret.isStreamRepeat()) {
    //         for down, i in self.downstream():

    //             if self._drop:
    //                 if len(down._input[i]) > 0:
    //                     # do nothing
    //                     pass

    //                 elif not isinstance(down._active[i], StreamNone):
    //                     # do nothing
    //                     pass

    //                 else:
    //                     await down._push(ret, i)

    //             elif self._replace:
    //                 if len(down._input[i]) > 0:
    //                     _ = await down._pop(i)

    //                 elif not isinstance(down._active[i], StreamNone):
    //                     down._active[i] = ret

    //                 else:
    //                     await down._push(ret, i)

    //             else:
    //                 await down._push(ret, i)
    }
    return ret;
  }

  friend T_EXPORT ostream& operator<<(ostream& ostream, const Node<Function, Args...>& node) {
    ostream << node.name << "[" << node.id.substr(0, 6) << "]";
    return ostream;
  }

protected:
  // Name of the node
  t_str name;

  // Unique ID of the node
  t_str id;

  // Stored function
  t_func<ReturnType(Args...)> func;

  // Last value of function
  ValueType _last;

private:
  bool _backpressure() const {
    // check if downstream() are all empty, if not then don't propogate
    for (auto input: _input) {
      if (input.size() != 0) {
        return true;
      }
    }
    return false;
  }

  bool _finished = false;
  chrono::nanoseconds _delay_interval = 0ns;
  int _execution_count = 0;
  int _execution_max = 0;

  
  static constexpr size_t Inputs = sizeof...(Args);

  vector<ValueType> _active;
  vector<deque<ValueType>> _input;
  vector<t_nodeptr> _upstream;
  vector<pair<t_nodeptr, int>> _downstream;
};


template <typename Function, typename... Args>
class T_EXPORT Foo : public Node<Function, Args...> {
public:
  template <typename... Ts>
  Foo(Function _func, tuple<Ts...> funcArgs)
    : Node<Function, Args...>(bind(_func, funcArgs)) {}

protected:
};

} // namespace streaming
} // namespace tributary
