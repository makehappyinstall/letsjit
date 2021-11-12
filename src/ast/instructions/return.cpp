#include <ast/instructions/return.hpp>
#include <ast/expressions/expression.hpp>

namespace letsjit::ast {

void ReturnExpr::CompileInstruction(const letsjit::compilation::Context& context) const {
  const auto& ret_expr = GetValue().Compile(context);
  auto& ir_builder = context.GetIRBuilder();
  ir_builder.CreateRet(ret_expr.value);
}

}
