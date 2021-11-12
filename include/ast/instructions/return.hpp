#pragma once

#include <ast/instructions/instruction.hpp>

namespace letsjit::ast {

struct AbstractExpression;

struct ReturnExpr : AbstractInstruction {

  virtual const AbstractExpression& GetValue() const = 0;

  bool IsTerminalInstruction() const override { return true; }

  virtual void CompileInstruction(const letsjit::compilation::Context&) const override;
};

}