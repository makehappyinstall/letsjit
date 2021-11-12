#include <ast/nodes/ternary_node.hpp>

namespace letsjit::ast::nodes {

std::shared_ptr<TernaryNode>
MakeTernary(std::shared_ptr<AbstractExpression> cond,
            std::shared_ptr<AbstractExpression> if_true,
            std::shared_ptr<AbstractExpression> if_false) {
  return std::make_shared<TernaryNode>(cond, if_true, if_false);
}

} // namespace letsjit::ast::nodes