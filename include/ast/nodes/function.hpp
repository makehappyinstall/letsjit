#pragma once

#include <ast/instructions/function_definition.hpp>

namespace letsjit::ast::nodes {

struct FunctionNode :  public FunctionDefinition, public Node {
  FunctionNode(const FunctionDeclaration& declaration, std::shared_ptr<AbstractInstruction> body) : decl_(declaration), body_(body) {}
  FunctionDeclaration decl_;
  std::shared_ptr<AbstractInstruction> body_;

  virtual FunctionDeclaration GetDeclaration() const override {
    return decl_;
  }

  virtual const AbstractInstruction& GetBody() const override {
    return *body_;
  }
};

}