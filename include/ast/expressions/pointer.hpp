#pragma once
#include "expression.hpp"
namespace letsjit::ast {


struct PointerHolder {
  void* value;
  AnytypeHolder type;

  template <typename T>
  AnyPointerHolder(T* value) : value(static_cast<void*>(value)), type(MakeTypeHolder<T*>()) {}
};

struct PointerExpression : public AbstractExpression {
  virtual const AbstractExpression& GetPointerValue() const = 0;
  virtual CompiledValue Compile(const letsjit::compilation::Context &) const override;
};

}