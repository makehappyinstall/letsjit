#include <ast/nodes/function_arg.hpp>

namespace letsjit::ast::nodes {

std::shared_ptr<FunctionArgAccessNode>
MakeFunctionArg(const std::string &name) {
  return std::make_shared<FunctionArgAccessNode>(name);
}

} // namespace letsjit::ast::nodes