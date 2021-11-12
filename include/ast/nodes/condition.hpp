#include <ast/instructions/condition.hpp>

namespace letsjit::ast::nodes {

struct ConditionNode :  public ConditionInstruction, public Node {
  std::shared_ptr<AbstractExpression> cond_;
  std::shared_ptr<AbstractInstruction> if_true_, if_false_;
  ConditionNode(std::shared_ptr<AbstractExpression> cond, std::shared_ptr<AbstractInstruction> if_true, std::shared_ptr<AbstractInstruction> if_false = nullptr) :
  cond_(std::move(cond)), if_true_(std::move(if_true)), if_false_(std::move(if_false)) {}

  const AbstractExpression & GetCond() const override {
    return *cond_;
  }

  const AbstractInstruction & GetIfTrue() const override {
    return *if_true_;
  }

  const std::optional<AbstractInstruction *> GetIfFalse() const override {
    if (!if_false_) {
      return std::nullopt;
    }
    return {if_false_.get()};
  }
};

}