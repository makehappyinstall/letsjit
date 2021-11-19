#pragma once

#include <llvm/IR/Value.h>

#include <ast/compiled_value.hpp>
#include <ast/types/typeinfo.hpp>
#include <compilation/context.hpp>

namespace letsjit::ast {

struct AbstractExpression {
  virtual CompiledValue Compile(const letsjit::compilation::Context&) const = 0;
  virtual ~AbstractExpression();
};

struct VoidExpression : AbstractExpression {
  CompiledValue Compile(const letsjit::compilation::Context &) const override {
    return {nullptr};
  }
};

extern VoidExpression kVoidValue;

}