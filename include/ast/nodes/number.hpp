#pragma once

#include <ast/expressions/number.hpp>
#include <ast/nodes/node.hpp>

namespace letsjit::ast::nodes {

template <typename T>
struct Number : public NumberExpression, public Node {
  T value;

  explicit Number(T value) : value(value) {  }

  virtual std::unique_ptr<AnyNumber> Get() const override {
    return std::make_unique<NumberHolder<T>>(value);
  }
};

template <typename T>
std::shared_ptr<Number<T>> MakeNumber(T value) {
  return std::make_shared<Number<T>>(value);
}

}