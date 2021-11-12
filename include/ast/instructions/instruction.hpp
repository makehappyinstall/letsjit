#pragma once

#include "compilation/context.hpp"

namespace letsjit::ast {

struct AbstractInstruction {
  virtual void CompileInstruction(const letsjit::compilation::Context&) const = 0;
  virtual bool IsTerminalInstruction() const = 0;
};

}