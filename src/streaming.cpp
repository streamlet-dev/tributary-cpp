
#include <string>
#include <tributary/base.h>
#include <tributary/streaming.h>
// #include <boost/algorithm/string/replace.hpp>
// #include <boost/uuid/uuid.hpp>
// #include <boost/uuid/uuid_generators.hpp>
// #include <boost/uuid/uuid_io.hpp>

namespace tributary {
namespace streaming {

// std::string
// Node::generateUUID() {
//   static boost::uuids::random_generator generator;
//   std::string uuid = boost::uuids::to_string(generator());
//   boost::replace_all(uuid, "-", "");
//   return uuid;
// }

// template <typename Ret, typename ...Args>
// std::ostream&
// operator<<(std::ostream& ostream, const Node<Ret, Args&...>& node) {
//   ostream << node.name << "[" << node.id.substr(0, 6) << "]";
//   return ostream;
// };

} // namespace streaming
} // namespace tributary
