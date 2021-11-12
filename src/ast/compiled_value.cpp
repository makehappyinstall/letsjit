#include "ast/compiled_value.hpp"
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_os_ostream.h>
#include <sstream>

namespace letsjit::ast {

std::ostream &operator<<(std::ostream &os, const CompiledValue &value) {
  if (!value.value) {
    os << "void";
    return os;
  }
  llvm::raw_os_ostream ros{os};
  value.value->print(ros, true);
  return os;
}

std::string CompiledValue::ToString(bool for_debug) const {
  if (!value) {
    return "void";
  }
  std::ostringstream oss;
  llvm::raw_os_ostream ros{oss};
  value->print(ros, for_debug);
  return oss.str();
}

bool CompiledValue::IsSameType(const CompiledValue &other) const {
  return GetTypeInfo() == other.GetTypeInfo();
}

Typeinfo CompiledValue::GetTypeInfo() const {
  if (!value) {
    return Typeinfo{nullptr};
  }
  return Typeinfo{value->getType()};
}

} // namespace letsjit::ast