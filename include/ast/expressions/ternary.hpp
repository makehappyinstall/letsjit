#pragma once
#include "expression.hpp"

namespace letsjit::ast {

struct TernaryExpression : public AbstractExpression {
  virtual const AbstractExpression& GetCond() const = 0;
  virtual const AbstractExpression& GetIfTrue() const = 0;
  virtual const AbstractExpression& GetIfFalse() const = 0;
  virtual CompiledValue Compile(const letsjit::compilation::Context&) const override;
};

}