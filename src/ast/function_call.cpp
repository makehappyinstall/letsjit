#include "ast/expressions/function_call.hpp"

namespace letsjit::ast {

CompiledValue
FunctionCall::Compile(const letsjit::compilation::Context &ctx) const {
  const auto &finfo = ctx.GetFunction(GetFunctionName());
  const auto args = GetArgs();
  if (finfo.arg_map.size() != args.size()) {
    throw std::runtime_error("Invalid args count in function " +
                             finfo.declaration.name + " call");
  }
  std::vector<llvm::Value *> llvm_args;
  llvm_args.reserve(args.size());
  std::transform(args.begin(), args.end(), std::back_inserter(llvm_args),
                 [&ctx](const auto &arg) {
                   auto arg_value = arg->Compile(ctx);
                   return arg_value.value;
                 });
  return {ctx.GetIRBuilder().CreateCall(finfo.function, llvm_args, "calltmp")};
}

void FunctionCall::CompileInstruction(
    const letsjit::compilation::Context &ctx) const {
  Compile(ctx);
}

bool FunctionCall::IsTerminalInstruction() const { return false; }

} // namespace letsjit::ast