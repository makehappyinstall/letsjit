#include "ast/expressions/function_arg_access.hpp"

namespace letsjit::ast {

CompiledValue
FunctionArgAccess::Compile(const letsjit::compilation::Context &ctx) const {
  const auto &current_fun = ctx.CurrentFunction();
  const auto &name = GetName();
  if (const auto it = current_fun.arg_map.find(name);
      it != current_fun.arg_map.end()) {
    return it->second;
  }
  throw std::runtime_error("Invalid argument " + name +
                           " requested from function " +
                           current_fun.declaration.name);
}

} // namespace letsjit::ast