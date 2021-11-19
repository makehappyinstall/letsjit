#pragma once

#include <memory>
#include <stack>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

#include <ast/types/typeinfo.hpp>
#include <ast/compiled_value.hpp>
#include <ast/types/types_context.hpp>

namespace llvm {
struct ExecutionEngine;
}

namespace letsjit {

namespace compilation {
struct Context;
}
}

namespace letsjit::compilation {

struct Context {
  Context();

  [[nodiscard]] ::llvm::LLVMContext& GetLLVMContext() const;
  [[nodiscard]] ::llvm::IRBuilder<>& GetIRBuilder() const;
  [[nodiscard]] ::llvm::Module& GetModule() const;
  [[nodiscard]] TypesContext& GetTypesContext() const;

  void EnterFunction(const std::string&) const;
  void ExitFunction() const;
  FunctionInfo CurrentFunction() const;

  std::string DumpIR() const;
  void Compile();

 private:
  std::unique_ptr<::llvm::LLVMContext> ctx = std::make_unique<::llvm::LLVMContext>();
  std::unique_ptr<::llvm::IRBuilder<>> ir_builder = std::make_unique<::llvm::IRBuilder<>>(*ctx);
  std::unique_ptr<::llvm::Module> module = std::make_unique<::llvm::Module>("main_module", *ctx);
  std::unique_ptr<letsjit::TypesContext> types_context = std::make_unique<TypesContext>(*ctx, *module);
  ::llvm::ExecutionEngine * execution_engine_ = nullptr;

  mutable std::stack<FunctionInfo*> call_stack;
};


std::unique_ptr<Context> MakeContext();

}