#include <ast/nodes/field_access_node.hpp>

namespace letsjit::ast::nodes {

std::shared_ptr<FieldAccessNode>
MakeFieldAccess(std::shared_ptr<AbstractExpression> pointer_expr,
                const std::string &field_name) {
  return std::make_shared<FieldAccessNode>(pointer_expr, field_name);
}

} // namespace letsjit::ast::nodes