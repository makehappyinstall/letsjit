#include <llvm/ADT/APInt.h>
#include <llvm/ADT/APSInt.h>
#include <llvm/IR/Constants.h>

#include "ast/expressions/arithmeric_op.hpp"
#include "ast/expressions/field_access.hpp"

namespace letsjit::ast {

CompiledValue
FieldAccess::Compile(const letsjit::compilation::Context &ctx) const {
  const auto &base_struct_expr = GetBaseExpression();
  const auto &field_name = GetFieldName();
  auto base_struct_ptr = base_struct_expr.Compile(ctx);
  auto type_info = base_struct_ptr.GetTypeInfo();
  if (!type_info.IsPointer()) {
    /* FIXME: think about non-pointer types */
    throw std::runtime_error(
        "Cannot apply field access to non-pointer types (passed " +
        type_info.ToString() + ")");
  }
  auto target_object = type_info.GetPointerTargetType();
  auto struct_desc = ctx.GetTypesContext().GetStruct(target_object.RawType());
  if (struct_desc.decl.fields.count(field_name) == 0) {
    throw std::runtime_error("requested unknown field " + field_name +
                             " of type " + struct_desc.decl.name);
  }
  const auto &field_desc = struct_desc.decl.fields.at(field_name);
  auto &ir_builder = ctx.GetIRBuilder();
  auto result = ir_builder.CreateAdd(
      base_struct_ptr.value,
      ::llvm::ConstantInt::get(
          ctx.GetLLVMContext(),
          ::llvm::APSInt(
              ::llvm::APInt(sizeof(field_desc.offset) * 8, field_desc.offset),
              false /* is_signed */)),
      "addptroffset");
  auto field_type_info =
      field_desc.field_type.ToTypeinfo(ctx.GetTypesContext());
  result->mutateType(field_type_info.MakePointerTo().RawType());
  if (!ctx.GetTypesContext().IsStruct(field_type_info.RawType())) {
    auto &ir_builder = ctx.GetIRBuilder();
    return {ir_builder.CreateLoad(result, "deref")};
  }
  return {result};
}

} // namespace letsjit::ast
