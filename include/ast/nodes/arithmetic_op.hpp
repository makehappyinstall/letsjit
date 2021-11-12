#pragma once

#include <ast/expressions/arithmeric_op.hpp>
#include <ast/nodes/node.hpp>

namespace letsjit::ast::nodes {


struct ArithmeticOpNode : public ArithmeticOp, public Node {
  const std::shared_ptr<AbstractExpression> lnode;
  const std::shared_ptr<AbstractExpression> rnode;
  const ArithmeticOperations op;
  ArithmeticOpNode(std::shared_ptr<AbstractExpression> lnode, std::shared_ptr<AbstractExpression> rnode, ArithmeticOperations op) : lnode(std::move(lnode)), rnode(std::move(rnode)), op(op) {}

  virtual const AbstractExpression &GetLeft() const {
    return *lnode;
  }

  virtual const AbstractExpression &GetRight() const {
    return *rnode;
  }

  virtual ArithmeticOperations GetOp() const {
    return op;
  }
};

std::shared_ptr<ArithmeticOpNode> Plus(std::shared_ptr<AbstractExpression> lnode, std::shared_ptr<AbstractExpression> rnode);

std::shared_ptr<ArithmeticOpNode> Mul(std::shared_ptr<AbstractExpression> lnode, std::shared_ptr<AbstractExpression> rnode);

std::shared_ptr<ArithmeticOpNode> Sub(std::shared_ptr<AbstractExpression> lnode, std::shared_ptr<AbstractExpression> rnode);

std::shared_ptr<ArithmeticOpNode> Less(std::shared_ptr<AbstractExpression> lnode, std::shared_ptr<AbstractExpression> rnode);

std::shared_ptr<ArithmeticOpNode> LessEq(std::shared_ptr<AbstractExpression> lnode, std::shared_ptr<AbstractExpression> rnode);

std::shared_ptr<ArithmeticOpNode> Eq(std::shared_ptr<AbstractExpression> lnode, std::shared_ptr<AbstractExpression> rnode);

std::shared_ptr<ArithmeticOpNode> Greater(std::shared_ptr<AbstractExpression> lnode, std::shared_ptr<AbstractExpression> rnode);

std::shared_ptr<ArithmeticOpNode> GreaterEq(std::shared_ptr<AbstractExpression> lnode, std::shared_ptr<AbstractExpression> rnode);

}