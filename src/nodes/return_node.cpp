#include <ast/nodes/return.hpp>

namespace letsjit::ast::nodes {

std::shared_ptr<ReturnNode> MakeReturn(std::shared_ptr<AbstractExpression> expr) {
  return std::make_shared<ReturnNode>(expr);
}

}