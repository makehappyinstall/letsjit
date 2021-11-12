#pragma once

#include <ast/instructions/instruction.hpp>

namespace letsjit::ast {

struct SequenceInstruction : public AbstractInstruction {
  virtual std::vector<AbstractInstruction*> GetInstructions() const = 0;
  bool IsTerminalInstruction() const override;
  void CompileInstruction(const letsjit::compilation::Context &) const override;
};

}