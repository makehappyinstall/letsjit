#include <ast/types/types_context.hpp>
#include <compilation/context.hpp>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/DynamicLibrary.h>

namespace letsjit {

void TypesContext::RegisterFunction(const FunctionInfo &function_info) const {
  functions->emplace(function_info.declaration.name,
                     std::make_unique<FunctionInfo>(function_info));
}

void TypesContext::RegisterExternalFunctionImpl(
    const FunctionDeclaration &fdecl, void *fun_ptr) {
  std::vector<llvm::Type *> args = fdecl.FetchArgTypes(*this);
  auto fun_type = llvm::FunctionType::get(
      fdecl.return_type.ToTypeinfo(*this).RawType(), args, false);
  FunctionInfo finfo{fdecl,
                     ::llvm::Function::Create(fun_type,
                                              ::llvm::Function::ExternalLinkage,
                                              fdecl.name, &module_),
                     {},
                     fun_ptr};
  size_t id = 0;
  for (auto &arg : finfo.function->args()) {
    arg.setName(fdecl.args[id].first);
    finfo.arg_map[arg.getName()] = {&arg};
    ++id;
  }
  RegisterFunction(finfo);
}

void TypesContext::RegisterStruct(const StructDeclaration &struct_decl) const {
  ::llvm::StructType *custom_struct_type =
      llvm::StructType::create(llvm_context_);
  custom_struct_type->setName(struct_decl.name);
  auto struct_info_ptr =
      std::make_shared<StructInfo>(StructInfo{struct_decl, custom_struct_type});
  type_to_struct->emplace(std::make_pair(custom_struct_type, struct_info_ptr));
  typeid_to_struct->emplace(
      std::make_pair(struct_decl.type_index, struct_info_ptr));
}

const StructInfo &TypesContext::GetStruct(const std::string &name) const {
  auto *struct_type = module_.getTypeByName(name);
  if (const auto it = type_to_struct->find(struct_type);
      it != type_to_struct->end()) {
    return *it->second;
  }
  throw std::runtime_error("Requested unknown struct type " + name);
}

const StructInfo &TypesContext::GetStruct(::llvm::Type *target_type) const {
  if (const auto it = type_to_struct->find(target_type);
      it != type_to_struct->end()) {
    return *it->second;
  }
  throw std::runtime_error("Requested unknown struct type");
}

const StructInfo &TypesContext::GetStruct(std::type_index type_id) const {
  if (const auto it = typeid_to_struct->find(type_id);
      it != typeid_to_struct->end()) {
    return *it->second;
  }
  throw std::runtime_error("Requested unknown struct type");
}

bool TypesContext::IsStruct(::llvm::Type *target_type) const {
  return type_to_struct->find(target_type) != type_to_struct->end();
}

bool TypesContext::IsStruct(std::type_index type_id) const {
  return typeid_to_struct->find(type_id) != typeid_to_struct->end();
}

FunctionInfo TypesContext::GetFunction(const std::string &name) const {
  return *GetFunctionPtr(name);
}

FunctionInfo *TypesContext::GetFunctionPtr(const std::string &name) const {
  if (const auto it = functions->find(name); it != functions->end()) {
    return it->second.get();
  }
  throw std::runtime_error("Unknown function " + name + " requested");
}

void TypesContext::InitGlobalFunctions() const {
  for (const auto &fun : *functions) {
    if (fun.second->fun_ptr) {
      ::llvm::sys::DynamicLibrary::AddSymbol(fun.first, fun.second->fun_ptr);
    }
  }
}

void TypesContext::InitLocalFunctions(
    llvm::ExecutionEngine *execution_engine) const {
  for (const auto &fun : *functions) {
    if (!fun.second->fun_ptr)
      fun.second->fun_ptr = reinterpret_cast<void *>(
          execution_engine->getFunctionAddress(fun.first));
  }
}

::llvm::LLVMContext &TypesContext::GetLLVMContext() const {
  return llvm_context_;
}

::llvm::LLVMContext &ExtractLLVMContext(const TypesContext &types_context) {
  return types_context.GetLLVMContext();
}

::llvm::Type *FetchStructType(const TypesContext &types_context,
                              std::type_index type_id) {
  if (!types_context.IsStruct(type_id))
    return nullptr;
  return types_context.GetStruct(type_id).type;
}

} // namespace letsjit
