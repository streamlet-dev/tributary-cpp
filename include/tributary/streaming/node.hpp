#pragma once
#include <chrono>
#include <deque>
#include <iostream>
#include <thread>
#include <utility>
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

  template <typename OtherFunction, typename... OtherArgs>
  friend class Node;

public:
  Node(Function _func)
    : name("Node")
    , id(generateUUID())
    , func(_func) {}

  auto value() {
    return _last;
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

    /*

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
          _active[i] = StreamNone::inst();
          ready = false;
        }
      }
    }

    */

    if (ready) {
      // execute function
      return _execute(forward<Args>(_args)...);
    }


    return _last;
  }

  template <typename OtherFunction, typename... OtherArgs>
  void pipe(Node<OtherFunction, OtherArgs...>& rhs) {
    // wire lhs into rhs
    _downstream.push_back(make_pair(&rhs, rhs._upstream.size()));
    rhs._upstream.push_back(this);
  }

  Node& operator>>(Node& rhs) {
    // wire lhs into rhs
    _downstream.push_back(make_pair(&rhs, rhs._upstream.size()));
    rhs._upstream.push_back(this);
    return rhs;
  }

  Node& operator<<(Node& rhs) {
    // wire rhs into lhs
    rhs._downstream.push_back(make_pair(this, _upstream.size()));
    _upstream.push_back(&rhs);
    return *this;
  }

  void _push(ValueType& inp, size_t index) {
    // push value to downstream nodes
    _input[index].push_back(inp);
  }

  bool _empty(size_t index) {
    // check if value
    return _input[index].size() == 0 || _active[index].isNone();
  }

  ValueType _pop(size_t index) {
    // pop value from downstream nodes
    if (_input[index].size() > 0) {
      auto ret = _input[index].front();
      _input[index].pop_front();
      return ret;
    }

    return StreamNone::inst();
  }


  ValueType
  _execute(Args... _args) {
    // execute callable
    // assume no valid input
    bool valid = false;

    ValueType nextLast;

    // wait for valid input
    while (!valid) {
      // await if its a coroutine
      // if asyncio.iscoroutine(self._func):
      //     nextLast = await self._func(*self._active, **self._func_kwargs)

      //         # else call it
      //         elif isinstance(self._func, types.FunctionType):
      //             try:
      //                 # could be a generator
      //                 try:
        nextLast = func(forward<Args>(_args)...);
      //                     nextLast = self._func(*self._active, **self._func_kwargs)
      //                 except ZeroDivisionError:
      //                     nextLast = float("inf")

      //             except ValueError:
      //                 # Swap back to function to get a new generator next iteration
      //                 self._func = self._old_func
      //                 continue

      //         else:
      //             raise TributaryException("Cannot use type:{}".format(type(self._func)))

      // calculation was valid
      valid = true;

      // increment execution count
      _execution_count += 1;
    }

  //     if isinstance(nextLast, types.AsyncGeneratorType):

  //         async def _func(g=nextLast):
  //             return await _agen_to_func(g)

  //         self._func = _func
  //         nextLast = await self._func()

  //     elif isinstance(nextLast, types.GeneratorType):
  //         # Swap to generator unroller
  //         self._old_func = self._func
  //         self._func = lambda g=nextLast: _gen_to_func(g)
  //         nextLast = self._func()

  //     elif asyncio.iscoroutine(nextLast):
  //         nextLast = await nextLast

  //     if self._repeat:
  //         if isinstance(_last, (StreamNone, StreamRepeat)):
  //             # NOOP
  //             self._last = self._last
  //         else:
  //             self._last = nextLast
  //     else:
    _last = nextLast;

    _output(_last);

    for (auto i = 0; i < _active.size(); ++i) {
      _active[i] = StreamNone::inst();
    }

    if (_last.isStreamEnd()) {
      _finish();
    }
    return _last;
  }

  ValueType
  _finish() {
    // mark this node as finished
    _finished = true;
    _last = StreamEnd::inst(); // FIXME
    return _output(_last);
  }


  ValueType
  _output(ValueType ret) {
    // output value to downstream nodes
    // if downstreams, output
    if (!ret.isStreamNone() && !ret.isStreamRepeat()) {
      for (auto j = 0; j< _downstream.size(); ++j) {
        auto downpair = _downstream[j];
        auto down = std::any_cast<Node>(downpair.first);
        auto i = downpair.second;

        if(_drop) {
          if (down._input[i].size() > 0) {
            // do nothing
          } else if (!down._active[i].isStreamNone()) {
            // do nothing
          } else {
            down._push(ret, i);
          }
        } else if (_replace) {
          if(down._input[i].size() > 0) {
            down._pop(i);
          } else if (!down._active[i].isStreamNone()) {
            down._active[i] = ret;
          } else {
            down._push(ret, i);
          }
        } else {
          down._push(ret, i);
        }
      }
    }
    return ret;
  }

  friend T_EXPORT ostream& operator<<(ostream& ostream, const Node<Function, Args...>& node) {
    ostream << node.name << "[" << node.id.substr(0, 6) << "]";
    return ostream;
  }


public:
  vector<ValueType> _active;
  vector<deque<ValueType>> _input;
  vector<std::any> _upstream;
  vector<pair<std::any, int>> _downstream;

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
    if (_drop || _replace) {
      return false;
    }

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

  // TODO
  int _execution_count = 0;
  int _execution_max = 0;
  bool _drop = false;
  bool _replace = false;
  
  // static constexpr size_t Inputs = sizeof...(Args);

  // vector<ValueType> _active;
  // vector<deque<ValueType>> _input;
  // vector<Node*> _upstream;
  // vector<pair<Node*, int>> _downstream;
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
