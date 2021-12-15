#pragma once

#include <functional>
#include <tributary/base.h>

namespace tributary {
namespace utils {

// https://stackoverflow.com/questions/9059187/equivalent-c-to-python-generator-pattern
static T_EXPORT std::function<int()> generator = []{
  int i = 0;
  return [=]() mutable {
    return i < 10 ? i++ : -1;
  };
}();

static T_EXPORT std::function<int()> addTo(int x){
    int i = 0;
    return [=]() mutable {   
        return i++ + x;
    };
};

T_EXPORT std::string generateUUID();

}
}
