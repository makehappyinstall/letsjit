#pragma once

namespace letsjit {

struct FieldDeclaration {
  size_t offset;
  letsjit::ast::AnytypeHolder field_type;

  template <typename T, typename R>
  FieldDeclaration(R T::*member_ptr, const TypesContext& ctx) {
    offset = reinterpret_cast<std::ptrdiff_t>(&(reinterpret_cast<T const volatile*>(NULL)->*member_ptr));
    field_type = letsjit::ast::MakeTypeHolder<R>(ctx);
  }
};

struct StructDeclaration {
  std::string name;
  std::unordered_map<std::string, FieldDeclaration> fields;
  std::type_index type_index;
};

}