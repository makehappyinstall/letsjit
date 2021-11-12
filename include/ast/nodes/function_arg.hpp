#pragma once

#include <ast/nodes/node.hpp>
#include <ast/expressions/function_arg_access.hpp>
namespace letsjit::ast::nodes {

struct FunctionArgAccessNode : public FunctionArgAccess, public Node {
  FunctionArgAccessNode(const std::string& name) : name_(name) {}

  std::string GetName() const override {
    return name_;
  }

  std::string name_;
};

std::shared_ptr<FunctionArgAccessNode> MakeFunctionArg(const std::string & name);

}