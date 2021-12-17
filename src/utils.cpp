#include <chrono>
#include <string>
#include <thread>
#include <boost/algorithm/string/replace.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cppcoro/task.hpp>
#include <tributary/utils.h>

using namespace std::chrono_literals;

namespace tributary {
namespace utils {

T_EXPORT
cppcoro::task<int>
asyncFunctionCoro() noexcept {
  static int value{0};
  std::this_thread::sleep_for(100ms);
  co_return value += 1;
}

T_EXPORT cppcoro::generator<int>
asyncGenerator() {
  int x = 0;
  while (true)
    co_yield x++;
}

T_EXPORT std::function<int()>
addTo(int x) {
  int i = 0;
  return [=]() mutable { return i++ + x; };
};

T_EXPORT std::string
generateUUID() {
  static boost::uuids::random_generator uuidGenerator;
  std::string uuid = boost::uuids::to_string(uuidGenerator());
  boost::replace_all(uuid, "-", "");
  return uuid;
};

} // namespace utils
} // namespace tributary
