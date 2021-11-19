#pragma once
#include <ast/expressions/field_access.hpp>
#include <ast/nodes/node.hpp>

namespace letsjit::ast::nodes {

struct FieldAccessNode : public FieldAccess, public Node {
  std::string GetFieldName() const override {
    return field_name;
  }

  const AbstractExpression & GetBaseExpression() const override {
    return *pointer_expr;
  }
  FieldAccessNode(std::shared_ptr<AbstractExpression> pointer_expr, const std::string& field_name) : field_name(field_name), pointer_expr(std::move(pointer_expr)) {}

  std::string field_name;
  std::shared_ptr<AbstractExpression> pointer_expr;
};

std::shared_ptr<FieldAccessNode> MakeFieldAccess(std::shared_ptr<AbstractExpression> pointer_expr, const std::string& field_name);

}