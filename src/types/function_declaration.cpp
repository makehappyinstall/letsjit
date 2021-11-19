#include <ast/types/function_declaration.hpp>

namespace letsjit {

std::vector<llvm::Type *>
FunctionDeclaration::FetchArgTypes(TypesContext &types_context) const {
  std::vector<llvm::Type *> result{};
  result.reserve(args.size());
  std::transform(args.begin(), args.end(), std::back_inserter(result),
                 [&types_context](const auto &arg) {
                   return arg.second.ToTypeinfo(types_context).RawType();
                 });
  return result;
}

} // namespace letsjit
