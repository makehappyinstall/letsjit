#include "llvm/IR/Verifier.h"

#include "ast/instructions/function_definition.hpp"

namespace letsjit::ast {

void FunctionDefinition::CompileInstruction(const letsjit::compilation::Context & context) const {
  auto decl = GetDeclaration();
  std::vector<::llvm::Type*> arg_list = decl.FetchArgTypes(context);
  auto ret_type = decl.return_type.ToTypeinfo(context.GetLLVMContext());
  ::llvm::FunctionType* func_type = ::llvm::FunctionType::get(ret_type.RawType(), arg_list, false);
  auto fun_ptr = ::llvm::Function::Create(func_type, ::llvm::Function::ExternalLinkage, decl.name, &context.GetModule());
  size_t id = 0;
  FunctionInfo finfo{decl, fun_ptr, {}};
  for (auto& arg: fun_ptr->args()) {
    arg.setName(decl.args[id].first);
    finfo.arg_map[arg.getName()] = {&arg};
    ++id;
  }

  context.RegisterFunction(finfo);

  auto block = ::llvm::BasicBlock::Create(context.GetLLVMContext(), "entry", fun_ptr);
  auto& ir_builder = context.GetIRBuilder();
  ir_builder.SetInsertPoint(block);
  context.EnterFunction(decl.name);
  GetBody().CompileInstruction(context);
  context.ExitFunction();
  ::llvm::verifyFunction(*fun_ptr);
}

}