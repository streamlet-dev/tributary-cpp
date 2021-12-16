#pragma once

#include <functional>
#include <tributary/base.h>

namespace tributary {
namespace utils {

// https://stackoverflow.com/questions/9059187/equivalent-c-to-python-generator-pattern
static std::function<int()> generator = [] {
  int i = 0;
  return [=]() mutable { return i < 10 ? i++ : -1; };
}();

static std::function<int()>
addTo(int x) {
  int i = 0;
  return [=]() mutable { return i++ + x; };
};

T_EXPORT std::string generateUUID();


class T_EXPORT BaseNode {
protected:
  BaseNode()=default;
};

class T_EXPORT StreamNone : public BaseNode {
public:
  StreamNone& inst() {
    static StreamNone inst;
    return inst;
  }
private:
  StreamNone()=default;
  ~StreamNone()=default;
};


class T_EXPORT StreamBegin : public BaseNode {
public:
  StreamBegin& inst() {
    static StreamBegin inst;
    return inst;
  }
private:
  StreamBegin()=default;
  ~StreamBegin()=default;
};

class T_EXPORT StreamEnd : public BaseNode {
public:
  StreamEnd& inst() {
    static StreamEnd inst;
    return inst;
  }
private:
  StreamEnd()=default;
  ~StreamEnd()=default;
};

} // namespace utils
} // namespace tributary
