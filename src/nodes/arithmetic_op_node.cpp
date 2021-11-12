#include <ast/nodes/arithmetic_op.hpp>

namespace letsjit::ast::nodes {

std::shared_ptr<ArithmeticOpNode>
Plus(std::shared_ptr<AbstractExpression> lnode,
     std::shared_ptr<AbstractExpression> rnode) {
  return std::make_shared<ArithmeticOpNode>(lnode, rnode,
                                            ArithmeticOperations::kPlus);
}

std::shared_ptr<ArithmeticOpNode>
Mul(std::shared_ptr<AbstractExpression> lnode,
    std::shared_ptr<AbstractExpression> rnode) {
  return std::make_shared<ArithmeticOpNode>(lnode, rnode,
                                            ArithmeticOperations::kMul);
}

std::shared_ptr<ArithmeticOpNode>
Sub(std::shared_ptr<AbstractExpression> lnode,
    std::shared_ptr<AbstractExpression> rnode) {
  return std::make_shared<ArithmeticOpNode>(lnode, rnode,
                                            ArithmeticOperations::kMinus);
}

std::shared_ptr<ArithmeticOpNode>
Less(std::shared_ptr<AbstractExpression> lnode,
     std::shared_ptr<AbstractExpression> rnode) {
  return std::make_shared<ArithmeticOpNode>(lnode, rnode,
                                            ArithmeticOperations::kLess);
}

std::shared_ptr<ArithmeticOpNode>
LessEq(std::shared_ptr<AbstractExpression> lnode,
       std::shared_ptr<AbstractExpression> rnode) {
  return std::make_shared<ArithmeticOpNode>(lnode, rnode,
                                            ArithmeticOperations::kLessEq);
}

std::shared_ptr<ArithmeticOpNode>
Eq(std::shared_ptr<AbstractExpression> lnode,
   std::shared_ptr<AbstractExpression> rnode) {
  return std::make_shared<ArithmeticOpNode>(lnode, rnode,
                                            ArithmeticOperations::kEq);
}

std::shared_ptr<ArithmeticOpNode>
Greater(std::shared_ptr<AbstractExpression> lnode,
        std::shared_ptr<AbstractExpression> rnode) {
  return std::make_shared<ArithmeticOpNode>(lnode, rnode,
                                            ArithmeticOperations::kGreater);
}

std::shared_ptr<ArithmeticOpNode>
GreaterEq(std::shared_ptr<AbstractExpression> lnode,
          std::shared_ptr<AbstractExpression> rnode) {
  return std::make_shared<ArithmeticOpNode>(lnode, rnode,
                                            ArithmeticOperations::kGreaterEq);
}

} // namespace letsjit::ast::nodes