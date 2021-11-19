#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <ast/types/types_context_fwd.hpp>

namespace letsjit::ast {

struct Typeinfo {
  explicit Typeinfo(::llvm::Type* base_type) : type_(base_type) {}

  ::llvm::Type* RawType() const {
    return type_;
  }

  bool operator==(const Typeinfo& other) {
    if (!type_ && !other.type_) {
      return true;
    }
    if (!type_ || !other.type_) {
      return false;
    }
    return type_->getTypeID() == other.RawType()->getTypeID();
  }

  bool IsSigned() {
    if (!type_)
      return false;
    return type_->isSingleValueType();
  }

  bool IsPointer() const {
    if (!type_)
      return false;
    return type_->isPointerTy();
  }

  Typeinfo GetPointerTargetType() const {
    return Typeinfo{type_->getPointerElementType()};
  }

  Typeinfo MakePointerTo() const {
    return Typeinfo{llvm::PointerType::getUnqual(type_)};
  }

  std::string ToString() const;

 private:
  ::llvm::Type* type_;
};

std::ostream& operator<<(std::ostream& os, const Typeinfo& tinfo);

struct AbstractTypeHolder {
  virtual ::llvm::Type* MakeRawType(TypesContext& context) = 0;
  virtual ~AbstractTypeHolder() {}

  Typeinfo ToTypeinfo(TypesContext& context) {
    return Typeinfo{MakeRawType(context)};
  }
};

template <typename T>
struct TypeHolder;

template<>
struct TypeHolder<void> : AbstractTypeHolder{
  ::llvm::Type* MakeRawType(TypesContext& context) {
    return llvm::Type::getVoidTy(ExtractLLVMContext(context));
  }
};

template<>
struct TypeHolder<int> : AbstractTypeHolder{
  ::llvm::Type* MakeRawType(TypesContext& context) {
    return ::llvm::Type::getInt32Ty(ExtractLLVMContext(context));
  }
};

template<>
struct TypeHolder<double> : AbstractTypeHolder{
  ::llvm::Type* MakeRawType(TypesContext& context) {
    return ::llvm::Type::getDoubleTy(ExtractLLVMContext(context));
  }
};

template <class T, std::size_t = sizeof(T)>
std::true_type has_some_impl_(T *);

std::false_type has_some_impl_(...);

template <typename T>
constexpr bool has_static_type_holder_v = decltype(has_some_impl_(std::declval<TypeHolder<T>*>()))::value;

template <typename T>
struct TypeHolder<T*> : AbstractTypeHolder {
  ::llvm::Type* MakeRawType(TypesContext& context) {
    if constexpr (has_static_type_holder_v<T>) {
      TypeHolder<T> target_type{};
      return ::llvm::PointerType::get(target_type.MakeRawType(context));
    } else {
      /* Lets check runtime types */
      if (auto struct_type = FetchStructType<T>(context); struct_type != nullptr) {
        return llvm::PointerType::getUnqual(struct_type);
      }
      throw std::runtime_error(std::string{"Unknown type requested ("} + typeid(T).name() + ")");
    }
  }
};

struct DynamicTypeHolder : AbstractTypeHolder {
  ::llvm::Type* type_;

  DynamicTypeHolder(::llvm::Type* type) : type_(type) {}

  ::llvm::Type* MakeRawType(TypesContext&) {
    return type_;
  }
};

struct AnytypeHolder {
  std::shared_ptr<AbstractTypeHolder> type_holder_;

  Typeinfo ToTypeinfo(TypesContext& context) const {
    return type_holder_->ToTypeinfo(context);
  }
};

template <typename T>
AnytypeHolder MakeTypeHolder() {
  return AnytypeHolder{std::static_pointer_cast<AbstractTypeHolder>(std::make_shared<TypeHolder<T>>())};
}

template <typename T>
AnytypeHolder MakeTypeHolder(const TypesContext& ctx) {
  if constexpr (has_static_type_holder_v<T>) {
    return MakeTypeHolder<T>();
  } else {
    return AnytypeHolder{std::make_shared<DynamicTypeHolder>(FetchStructType<T>(ctx))};
  }
}

}