#pragma once

#include "expression.hpp"

namespace letsjit::ast {

enum class ArithmeticOperations {
  kPlus,
  kMinus,
  kMul,
  kDiv,
  kLess,
  kLessEq,
  kEq,
  kGreater,
  kGreaterEq
};

struct ArithmeticOp : public AbstractExpression {

  virtual const AbstractExpression &GetLeft() const = 0;
  virtual const AbstractExpression &GetRight() const = 0;
  virtual ArithmeticOperations GetOp() const = 0;

  virtual CompiledValue Compile(const letsjit::compilation::Context &) const override;
};

}
