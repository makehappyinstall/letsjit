#pragma once

#include <ast/types/typeinfo.hpp>
#include <ast/types/function_declaration.hpp>
#include <ast/compiled_value.hpp>
#include <ast/types/struct_declaration.hpp>
#include <typeindex>

namespace llvm {
class ExecutionEngine;
}

namespace letsjit {

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

struct StructInfo {
  StructDeclaration decl;
  ::llvm::Type* type;
};

struct TypesContext {

  TypesContext(::llvm::LLVMContext& llvm_context, ::llvm::Module& module) : llvm_context_(llvm_context), module_(module) {}

  void RegisterFunction(const FunctionInfo& function_info) const;
  void RegisterStruct(const StructDeclaration& struct_info) const;
  const StructInfo& GetStruct(const std::string& name) const;
  const StructInfo& GetStruct(::llvm::Type* struct_type) const;
  const StructInfo& GetStruct(std::type_index type_id) const;
  bool IsStruct(::llvm::Type* struct_type) const;
  bool IsStruct(std::type_index type_id) const;
  ::llvm::LLVMContext& GetLLVMContext() const;

  template <typename R, typename ... Args>
  void RegisterExternalFunction(const std::string& name, R(*target)(Args...)) {
    const auto fdecl = MakeFunctionDeclaration<R, Args...>(name, *this);
    RegisterExternalFunctionImpl(fdecl, reinterpret_cast<void*>(target));
  }

  template <typename R, typename ... Args>
  void RegisterExternalFunction(const FunctionDeclaration& fdecl, R(*target)(Args...)) {
    const size_t arg_size = sizeof...(Args);
    if (fdecl.args.size() != arg_size) {
      throw std::runtime_error("arguments count mismatches");
    }
    // TODO: add type checker
    RegisterExternalFunctionImpl(fdecl, reinterpret_cast<void*>(target));
  }

  FunctionInfo GetFunction(const std::string& name) const;
  FunctionInfo* GetFunctionPtr(const std::string& name) const;
  void InitGlobalFunctions() const;
  void InitLocalFunctions(llvm::ExecutionEngine*) const;

 private:
  ::llvm::LLVMContext& llvm_context_;
  ::llvm::Module& module_;
  std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<FunctionInfo>>> functions = std::make_unique<std::unordered_map<std::string, std::unique_ptr<FunctionInfo>>>();
  std::unique_ptr<std::unordered_map<::llvm::Type*, std::shared_ptr<StructInfo>>> type_to_struct = std::make_unique<std::unordered_map<::llvm::Type*, std::shared_ptr<StructInfo>>>();
  std::unique_ptr<std::unordered_map<std::type_index, std::shared_ptr<StructInfo>>> typeid_to_struct = std::make_unique<std::unordered_map<std::type_index, std::shared_ptr<StructInfo>>>();

  void RegisterExternalFunctionImpl(const FunctionDeclaration& fdecl, void* function);
};

::llvm::LLVMContext& ExtractLLVMContext(const TypesContext& types_context);

::llvm::Type* FetchStructType(const TypesContext& types_context, std::type_index type_id);

}
