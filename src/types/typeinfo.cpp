#include <sstream>

#include <ast/types/typeinfo.hpp>
#include <llvm/Support/raw_os_ostream.h>

namespace letsjit::ast {

std::ostream &operator<<(std::ostream &os, const Typeinfo &tinfo) {
  if (tinfo.RawType() == nullptr)
    return os;
  llvm::raw_os_ostream ros{os};
  tinfo.RawType()->print(ros);
  return os;
}

std::string Typeinfo::ToString() const {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

} // namespace letsjit::ast