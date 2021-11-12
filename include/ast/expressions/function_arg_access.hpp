#pragma once

#include "expression.hpp"

namespace letsjit::ast {

struct FunctionArgAccess : public AbstractExpression {
  virtual std::string GetName() const = 0;
  virtual CompiledValue Compile(const letsjit::compilation::Context &) const override;
};

}