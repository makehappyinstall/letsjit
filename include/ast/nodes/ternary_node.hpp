#pragma once
#include <ast/nodes/node.hpp>
#include <ast/expressions/ternary.hpp>

namespace letsjit::ast::nodes {

struct TernaryNode : public TernaryExpression, public Node {
  TernaryNode(std::shared_ptr<AbstractExpression> cond, std::shared_ptr<AbstractExpression> if_true, std::shared_ptr<AbstractExpression> if_false) : cond_(std::move(cond)), if_true_(std::move(if_true)), if_false_(std::move(if_false)) {

  }

  const AbstractExpression & GetCond() const override {
    return *cond_;
  }

  const AbstractExpression & GetIfFalse() const override {
    return *if_false_;
  }

  const AbstractExpression & GetIfTrue() const override {
    return *if_true_;
  }

  std::shared_ptr<AbstractExpression> cond_;
  std::shared_ptr<AbstractExpression> if_true_;
  std::shared_ptr<AbstractExpression> if_false_;
};

std::shared_ptr<TernaryNode> MakeTernary(std::shared_ptr<AbstractExpression> cond, std::shared_ptr<AbstractExpression> if_true, std::shared_ptr<AbstractExpression> if_false);

}