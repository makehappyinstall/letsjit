#pragma once

#include <ast/instructions/instruction.hpp>
#include <ast/instructions/function_definition.hpp>

namespace letsjit::ast {

struct FunctionCall : AbstractExpression, AbstractInstruction {
  virtual std::string GetFunctionName() const = 0;
  virtual std::vector<AbstractExpression*> GetArgs() const = 0;
  CompiledValue Compile(const letsjit::compilation::Context &) const override;
  void CompileInstruction(const letsjit::compilation::Context &) const override;
  bool IsTerminalInstruction() const override;

};

}