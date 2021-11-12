#include "ast/instructions/sequence.hpp"

namespace letsjit::ast {

void SequenceInstruction::CompileInstruction(
    const letsjit::compilation::Context &ctx) const {
  auto seq = GetInstructions();
  for (const auto *ptr : seq) {
    ptr->CompileInstruction(ctx);
  }
}

bool SequenceInstruction::IsTerminalInstruction() const {
  for (const auto &ptr : GetInstructions()) {
    if (ptr->IsTerminalInstruction())
      return true;
  }
  return false;
}

} // namespace letsjit::ast