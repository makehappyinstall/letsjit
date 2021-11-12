#pragma once

#include <ast/instructions/instruction.hpp>

namespace letsjit::ast {

struct AbstractExpression;

struct ConditionInstruction : public AbstractInstruction {
  virtual const AbstractExpression& GetCond() const = 0;
  virtual const AbstractInstruction& GetIfTrue() const = 0;
  virtual const std::optional<AbstractInstruction*> GetIfFalse() const = 0;

  bool IsTerminalInstruction() const override;

  void CompileInstruction(const letsjit::compilation::Context &) const override;
};

}