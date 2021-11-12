#pragma once

#include <ast/instructions/sequence.hpp>
#include <ast/nodes/node.hpp>

namespace letsjit::ast::nodes {

struct SequenceNode :  public SequenceInstruction, public Node {
  std::vector<std::shared_ptr<AbstractInstruction>> instructions_;

  SequenceNode() = default;
  SequenceNode(std::vector<std::shared_ptr<AbstractInstruction>> instructions) : instructions_(std::move(instructions)) {}

  std::vector<AbstractInstruction *> GetInstructions() const override {
    std::vector<AbstractInstruction *> result;
    std::transform(instructions_.begin(), instructions_.end(), std::back_inserter(result), [](const auto& ptr) { return ptr.get(); });
    return result;
  }
};

template <typename ...Args>
std::shared_ptr<SequenceNode> MakeSequence(std::shared_ptr<Args>... args) {
  static_assert(std::conjunction_v<std::is_base_of<AbstractInstruction, Args>...>, "Arguments must derive from AbstractInstruction");
  std::vector<std::shared_ptr<AbstractInstruction>> args_vec{args...};
  return std::make_shared<SequenceNode>(args_vec);
}

}