#pragma once

#include <memory>
#include <stack>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

#include <ast/typeinfo.hpp>
#include <ast/compiled_value.hpp>

namespace llvm {
struct ExecutionEngine;
}

namespace letsjit {

namespace compilation {
struct Context;
}

struct FunctionDeclaration {
  std::string name;
  std::vector<std::pair<std::string, letsjit::ast::AnytypeHolder>> args;
  letsjit::ast::AnytypeHolder return_type;

  std::vector<llvm::Type*> FetchArgTypes(const compilation::Context& ctx) const;
};

struct FunctionInfo {
  FunctionDeclaration declaration;
  ::llvm::Function* function;
  std::unordered_map<std::string, letsjit::ast::CompiledValue> arg_map;
  void* fun_ptr = nullptr;

  template <typename R, typename ... Args>
  R Call(Args...args) {
    using function_t = R(*)(Args...);
    assert(fun_ptr != nullptr);
    auto function = reinterpret_cast<function_t>(fun_ptr);
    return function(std::forward<Args...>(args)...);
  }
};

}

namespace letsjit::compilation {

namespace details {

template <typename ... Args>
std::vector<std::pair<std::string, letsjit::ast::AnytypeHolder>> MakeExternalFunctionParams() {
  std::vector<letsjit::ast::AnytypeHolder> types{letsjit::ast::MakeTypeHolder<Args>()...};
  std::vector<std::pair<std::string, letsjit::ast::AnytypeHolder>> result;
  result.reserve(types.size());
  size_t id = 0;
  std::transform(types.begin(), types.end(), std::back_inserter(result), [&id](const auto& type_holder) {
    return std::make_pair(std::string{"arg"} + std::to_string(id++), type_holder);
  });
  return result;
}

}

struct Context {
  Context();

  [[nodiscard]] ::llvm::LLVMContext& GetLLVMContext() const;
  [[nodiscard]] ::llvm::IRBuilder<>& GetIRBuilder() const;
  [[nodiscard]] ::llvm::Module& GetModule() const;

  void RegisterFunction(const FunctionInfo& function_info) const;

  template <typename R, typename ... Args>
  void RegisterExternalFunction(const std::string& name, R(*target)(Args...)) const {
    const FunctionDeclaration& fdecl{name, details::MakeExternalFunctionParams<Args...>(), letsjit::ast::MakeTypeHolder<R>()};
    RegisterExternalFunctionImpl(fdecl, reinterpret_cast<void*>(target));
  }

  template <typename R, typename ... Args>
  void RegisterExternalFunction(const FunctionDeclaration& fdecl, R(*target)(Args...)) const {
    const size_t arg_size = sizeof...(Args);
    if (fdecl.args.size() != arg_size) {
      throw std::runtime_error("arguments count mismatches");
    }
    // TODO: add type checker
    RegisterExternalFunctionImpl(fdecl, reinterpret_cast<void*>(target));
  }

  void EnterFunction(const std::string&) const;
  void ExitFunction() const;
  FunctionInfo CurrentFunction() const;
  FunctionInfo GetFunction(const std::string& name) const;

  std::string DumpIR() const;
  void Compile();

 private:
  std::unique_ptr<::llvm::LLVMContext> ctx = std::make_unique<::llvm::LLVMContext>();
  std::unique_ptr<::llvm::IRBuilder<>> ir_builder = std::make_unique<::llvm::IRBuilder<>>(*ctx);
  std::unique_ptr<::llvm::Module> module = std::make_unique<::llvm::Module>("main_module", *ctx);
  ::llvm::ExecutionEngine * execution_engine_ = nullptr;

  std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<FunctionInfo>>> functions = std::make_unique<std::unordered_map<std::string, std::unique_ptr<FunctionInfo>>>();
  mutable std::stack<FunctionInfo*> call_stack;
  void RegisterExternalFunctionImpl(const FunctionDeclaration& fdecl, void* function) const;
};


std::unique_ptr<Context> MakeContext();

}