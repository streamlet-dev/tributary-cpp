#include <string>
#include <boost/algorithm/string/replace.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <tributary/utils.h>

namespace tributary {
namespace utils {

T_EXPORT std::string
generateUUID() {
  static boost::uuids::random_generator uuidGenerator;
  std::string uuid = boost::uuids::to_string(uuidGenerator());
  boost::replace_all(uuid, "-", "");
  return uuid;
};

} // namespace utils
} // namespace tributary
