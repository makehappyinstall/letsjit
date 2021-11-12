#pragma once

#include <ast/expressions/expression.hpp>
#include <ast/nodes/node.hpp>
#include <ast/instructions/return.hpp>

namespace letsjit::ast::nodes {

struct ReturnNode : public ReturnExpr, public Node {
  std::shared_ptr<AbstractExpression> expr;
  ReturnNode(std::shared_ptr<AbstractExpression> expr=nullptr) : expr(std::move(expr)) {}
  virtual const AbstractExpression& GetValue() const override {
    if (!expr) {
      return kVoidValue;
    }
    return *expr;
  }
};

std::shared_ptr<ReturnNode> MakeReturn(std::shared_ptr<AbstractExpression> expr=nullptr);

}