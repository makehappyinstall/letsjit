#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>

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

 private:
  ::llvm::Type* type_;
};


struct AbstractTypeHolder {
  virtual ::llvm::Type* MakeRawType(::llvm::LLVMContext& context) = 0;
  virtual ~AbstractTypeHolder() {}

  Typeinfo ToTypeinfo(::llvm::LLVMContext& context) {
    return Typeinfo{MakeRawType(context)};
  }
};

template <typename T>
struct TypeHolder;

template<>
struct TypeHolder<void> : AbstractTypeHolder{
  ::llvm::Type* MakeRawType(::llvm::LLVMContext& context) {
    return llvm::Type::getVoidTy(context);
  }
};

template<>
struct TypeHolder<int> : AbstractTypeHolder{
  ::llvm::Type* MakeRawType(::llvm::LLVMContext& context) {
    return ::llvm::Type::getInt32Ty(context);
  }
};

template<>
struct TypeHolder<double> : AbstractTypeHolder{
  ::llvm::Type* MakeRawType(::llvm::LLVMContext& context) {
    return ::llvm::Type::getDoubleTy(context);
  }
};

struct AnytypeHolder {
  std::shared_ptr<AbstractTypeHolder> type_holder_;

  Typeinfo ToTypeinfo(::llvm::LLVMContext& context) const {
    return type_holder_->ToTypeinfo(context);
  }
};

template <typename T>
AnytypeHolder MakeTypeHolder() {
  return AnytypeHolder{std::make_shared<TypeHolder<T>>()};
}

}