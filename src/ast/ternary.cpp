#include "ast/expressions/ternary.hpp"

namespace letsjit::ast {

CompiledValue
TernaryExpression::Compile(const letsjit::compilation::Context &ctx) const {
  const auto &cond = GetCond();
  const auto &if_true = GetIfTrue();
  const auto &if_false = GetIfFalse();
  auto current_fun = ctx.GetIRBuilder().GetInsertBlock()->getParent();
  auto ThenBB =
      ::llvm::BasicBlock::Create(ctx.GetLLVMContext(), "then", current_fun);
  auto ElseBB = ::llvm::BasicBlock::Create(ctx.GetLLVMContext(), "else");
  auto MergeBB = ::llvm::BasicBlock::Create(ctx.GetLLVMContext(), "ifcont");
  auto &ir_builder = ctx.GetIRBuilder();
  auto cond_expr = cond.Compile(ctx);
  ir_builder.CreateCondBr(cond_expr.value, ThenBB, ElseBB);
  ir_builder.SetInsertPoint(ThenBB);
  auto if_true_val = if_true.Compile(ctx);
  ir_builder.CreateBr(MergeBB);
  current_fun->getBasicBlockList().push_back(ElseBB);
  ir_builder.SetInsertPoint(ElseBB);
  auto if_false_val = if_false.Compile(ctx);
  ir_builder.CreateBr(MergeBB);
  ElseBB = ir_builder.GetInsertBlock();
  current_fun->getBasicBlockList().push_back(MergeBB);
  ir_builder.SetInsertPoint(MergeBB);
  llvm::PHINode *phi =
      ir_builder.CreatePHI(if_true_val.value->getType(), 2, "iftmp");
  phi->addIncoming(if_true_val.value, ThenBB);
  phi->addIncoming(if_false_val.value, ElseBB);
  return {phi};
}

} // namespace letsjit::ast