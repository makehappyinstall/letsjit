#pragma once

#include <typeindex>

namespace letsjit {

struct TypesContext;
struct StructInfo;

::llvm::LLVMContext& ExtractLLVMContext(const TypesContext& types_context);

::llvm::Type* FetchStructType(const TypesContext& types_context, std::type_index type_id);

template <typename T>
::llvm::Type* FetchStructType(const TypesContext& types_context) {
  return FetchStructType(types_context, typeid(T));
}

}