#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/APSInt.h>
#include <llvm/IR/Constants.h>

#include <ast/expressions/number.hpp>

namespace letsjit::ast {

CompiledValue
NumberExpression::Compile(const letsjit::compilation::Context &context) const {
  auto value = Get();
  if (value->IsFloatingPoint()) {
    return CompiledValue{::llvm::ConstantFP::get(
        context.GetLLVMContext(), ::llvm::APFloat(value->AsDouble()))};
  }
  return CompiledValue{::llvm::ConstantInt::get(
      context.GetLLVMContext(),
      ::llvm::APSInt(
          ::llvm::APInt(value->UnderlyingBytes() * 8, value->AsLongInt()),
          !value->IsSigned()))};
}

} // namespace letsjit::ast