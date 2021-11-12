#include "ast/instructions/condition.hpp"

#include <ast/expressions/expression.hpp>

namespace letsjit::ast {

bool ConditionInstruction::IsTerminalInstruction() const {
  auto if_false = GetIfFalse();
  if (!if_false) {
    return false;
  }
  return GetIfTrue().IsTerminalInstruction() && (*if_false)->IsTerminalInstruction();
}

void ConditionInstruction::CompileInstruction(const letsjit::compilation::Context& ctx) const {
  const auto& cond = GetCond();
  const auto& if_true = GetIfTrue();
  const auto& if_false = GetIfFalse();
  auto current_fun = ctx.GetIRBuilder().GetInsertBlock()->getParent();
  auto ThenBB = ::llvm::BasicBlock::Create(ctx.GetLLVMContext(), "then", current_fun);
  llvm::BasicBlock* ElseBB = nullptr;
  llvm::BasicBlock* MergeBB = nullptr;
  if (if_false) {
    ElseBB = ::llvm::BasicBlock::Create(ctx.GetLLVMContext(), "else");
  }
  if (!IsTerminalInstruction()) {
    MergeBB = ::llvm::BasicBlock::Create(ctx.GetLLVMContext(), "ifcont");
  }
  auto& ir_builder = ctx.GetIRBuilder();
  auto cond_expr = cond.Compile(ctx);
  ir_builder.CreateCondBr(cond_expr.value, ThenBB, ElseBB?ElseBB:MergeBB);
  ir_builder.SetInsertPoint(ThenBB);
  if_true.CompileInstruction(ctx);
  if (!if_true.IsTerminalInstruction()) {
    ir_builder.CreateBr(MergeBB);
  }
  if (ElseBB) {
    current_fun->getBasicBlockList().push_back(ElseBB);
    ir_builder.SetInsertPoint(ElseBB);
    (*if_false)->CompileInstruction(ctx);
    if (MergeBB) {
      ir_builder.CreateBr(MergeBB);
    }
  }
  if (MergeBB) {
    ElseBB = ir_builder.GetInsertBlock();
    current_fun->getBasicBlockList().push_back(MergeBB);
    ir_builder.SetInsertPoint(MergeBB);
  }
}

}