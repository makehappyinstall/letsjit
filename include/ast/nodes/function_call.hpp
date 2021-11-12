#pragma once

#include <ast/expressions/function_call.hpp>

namespace letsjit::ast::nodes {

struct FunctionCallNode : public FunctionCall, public Node {
  std::string name_;
  std::vector<std::shared_ptr<AbstractExpression>> args_;
  FunctionCallNode(const std::string& function_name, std::vector<std::shared_ptr<AbstractExpression>> args) : name_(function_name), args_(std::move(args)) {}

  std::vector<AbstractExpression*> GetArgs() const override {
    std::vector<AbstractExpression*> result;
    result.reserve(args_.size());
    std::transform(args_.begin(), args_.end(), std::back_inserter(result), [](const auto& expr) {
      return expr.get();
    });
    return result;
  }

  std::string GetFunctionName() const override {
    return name_;
  }
};

template <typename ...Args>
std::shared_ptr<FunctionCallNode> MakeFunctionCall(const std::string& function_name, std::shared_ptr<Args>... args) {
  static_assert(std::conjunction_v<std::is_base_of<AbstractExpression, Args>...>, "Arguments must derive from AbstractExpression");
  std::vector<std::shared_ptr<AbstractExpression>> args_vec{args...};
  return std::make_shared<FunctionCallNode>(function_name, args_vec);
}

}