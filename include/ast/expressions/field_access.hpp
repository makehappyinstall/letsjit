#pragma once

#include <ast/expressions/expression.hpp>

namespace letsjit::ast {

struct FieldAccess : public AbstractExpression {
  virtual std::string GetFieldName() const = 0;
  virtual const AbstractExpression& GetBaseExpression() const = 0;

  CompiledValue Compile(const letsjit::compilation::Context &) const override;
};

}
