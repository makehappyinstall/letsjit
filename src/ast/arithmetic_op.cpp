#include "ast/expressions/arithmeric_op.hpp"
#include "ast/expressions/number.hpp"

namespace letsjit::ast {

CompiledValue
ArithmeticOp::Compile(const letsjit::compilation::Context &ctx) const {
  const auto &left =
      dynamic_cast<const AbstractExpression &>(GetLeft()).Compile(ctx);
  const auto &right =
      dynamic_cast<const AbstractExpression &>(GetRight()).Compile(ctx);
  if (!left.IsSameType(right)) {
    throw std::invalid_argument("Invalid arithmetic operation type");
  }
  auto &ir_builder = ctx.GetIRBuilder();
  // TODO: fix logic with signed and unsigned
  switch (GetOp()) {
  case ArithmeticOperations::kMinus:
    return {ir_builder.CreateSub(left.value, right.value, "subtmp")};
  case ArithmeticOperations::kMul:
    return {ir_builder.CreateMul(left.value, right.value, "multmp")};
  case ArithmeticOperations::kPlus:
    return {ir_builder.CreateAdd(left.value, right.value, "addtmp")};
  case ArithmeticOperations::kDiv: {
    if (left.GetTypeInfo().IsSigned() && right.GetTypeInfo().IsSigned()) {
      return {ir_builder.CreateExactSDiv(left.value, right.value, "divtmp")};
    } else {
      return {ir_builder.CreateExactUDiv(left.value, right.value, "divtmp")};
    }
  }
  case ArithmeticOperations::kEq:
    return {ir_builder.CreateICmpEQ(left.value, right.value, "eqtmp")};
  case ArithmeticOperations::kLess:
    if (left.GetTypeInfo().IsSigned() && right.GetTypeInfo().IsSigned()) {
      return {ir_builder.CreateICmpSLT(left.value, right.value, "lesstmp")};
    } else {
      return {ir_builder.CreateICmpULT(left.value, right.value, "lesstmp")};
    }
  case ArithmeticOperations::kLessEq:
    if (left.GetTypeInfo().IsSigned() && right.GetTypeInfo().IsSigned()) {
      return {ir_builder.CreateICmpSLE(left.value, right.value, "leqtmp")};
    } else {
      return {ir_builder.CreateICmpULE(left.value, right.value, "leqtmp")};
    }
  case ArithmeticOperations::kGreater:
    if (left.GetTypeInfo().IsSigned() && right.GetTypeInfo().IsSigned()) {
      return {ir_builder.CreateICmpSGT(left.value, right.value, "gttmp")};
    } else {
      return {ir_builder.CreateICmpUGT(left.value, right.value, "gttmp")};
    }
  case ArithmeticOperations::kGreaterEq:
    if (left.GetTypeInfo().IsSigned() && right.GetTypeInfo().IsSigned()) {
      return {ir_builder.CreateICmpSGE(left.value, right.value, "geqtmp")};
    } else {
      return {ir_builder.CreateICmpUGE(left.value, right.value, "geqtmp")};
    }
  }
}

} // namespace letsjit::ast