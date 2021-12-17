#include <chrono>
#include <thread>
#include <boost/algorithm/string/replace.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cppcoro/task.hpp>
#include <tributary/base.hpp>
#include <tributary/types.hpp>
#include <tributary/utils.hpp>

using namespace std;

namespace tributary {
namespace utils {

T_EXPORT t_fut<int> asyncFunctionCoro() {
  static int value{0};
  this_thread::sleep_for(100ms);
  co_return value += 1;
}

T_EXPORT t_gen<int> asyncGenerator() {
  int x = 0;
  while (true)
    co_yield x++;
}

T_EXPORT t_func<int()> addTo(int x) {
  int i = 0;
  return [=]() mutable { return i++ + x; };
};

T_EXPORT t_str generateUUID() {
  static boost::uuids::random_generator uuidGenerator;
  t_str uuid = boost::uuids::to_string(uuidGenerator());
  boost::replace_all(uuid, "-", "");
  return uuid;
};

} // namespace utils
} // namespace tributary
