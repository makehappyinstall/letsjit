#pragma once

#include <llvm/IR/Value.h>
#include <ast/typeinfo.hpp>

namespace letsjit::ast {

struct CompiledValue {
  ::llvm::Value* value;

  std::string ToString(bool for_debug=true) const;

  bool IsSameType(const CompiledValue& other) const;

  Typeinfo GetTypeInfo() const;
};

std::ostream& operator<<(std::ostream& os, const CompiledValue& value);


}
