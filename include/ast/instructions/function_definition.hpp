#pragma once

#include <string>
#include <vector>

#include <ast/typeinfo.hpp>
#include <ast/expressions/expression.hpp>
#include <ast/instructions/instruction.hpp>

namespace letsjit::ast {

struct FunctionDefinition : public AbstractInstruction {
  virtual FunctionDeclaration GetDeclaration() const = 0;
  virtual const AbstractInstruction& GetBody() const = 0;
  virtual void CompileInstruction(const letsjit::compilation::Context&) const override;
  bool IsTerminalInstruction() const override {
    return false;
  }
};

}