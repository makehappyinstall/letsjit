#pragma once

#include <ast/types/typeinfo.hpp>
#include <ast/types/types_context_fwd.hpp>

namespace letsjit::compilation {
struct Context;
}

namespace letsjit {

struct FunctionDeclaration {
  std::string name;
  std::vector<std::pair<std::string, letsjit::ast::AnytypeHolder>> args;
  letsjit::ast::AnytypeHolder return_type;

  std::vector<llvm::Type*> FetchArgTypes(TypesContext& types_context) const;
};


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

// TODO: remove copy-paste
template <typename ... Args>
std::vector<std::pair<std::string, letsjit::ast::AnytypeHolder>> MakeExternalFunctionParams(const std::vector<std::string>& arg_names) {
  std::vector<letsjit::ast::AnytypeHolder> types{letsjit::ast::MakeTypeHolder<Args>()...};
  std::vector<std::pair<std::string, letsjit::ast::AnytypeHolder>> result;
  result.reserve(types.size());
  if (arg_names.size() != types.size()) {
    throw  std::invalid_argument("Argument names and type count differs");
  }
  size_t id = 0;
  std::transform(types.begin(), types.end(), std::back_inserter(result), [&id, &arg_names](const auto& type_holder) {
    return std::make_pair(arg_names[id++], type_holder);
  });
  return result;
}

}

template <typename R, typename ... Args>
FunctionDeclaration MakeFunctionDeclaration(const std::string& name, const TypesContext& ctx) {
  return {name, details::MakeExternalFunctionParams<Args...>(), letsjit::ast::MakeTypeHolder<R>(ctx)};
}

template <typename R, typename ... Args>
FunctionDeclaration MakeFunctionDeclaration(const std::string& name, const std::vector<std::string>& arg_names, const TypesContext& ctx) {
  return {name, details::MakeExternalFunctionParams<Args...>(arg_names), letsjit::ast::MakeTypeHolder<R>(ctx)};
}


}