#pragma once

#include <type_traits>

#include "expression.hpp"

namespace letsjit::ast {


struct AnyNumber {
  virtual ~AnyNumber() {}
  virtual bool IsSigned() = 0;
  virtual bool IsFloatingPoint() = 0;
  virtual int UnderlyingBytes() = 0;
  virtual double AsDouble() = 0;
  virtual uint64_t AsLongInt() = 0;
};

namespace details {

template <typename T>
constexpr bool is_number = std::is_integral_v<T> || std::is_floating_point_v<T>;

}

template<typename T, typename=std::enable_if_t<details::is_number<T>,void>>
struct NumberHolder : public AnyNumber {
  T value;
  NumberHolder(T value) : value(value) {}

  virtual bool IsSigned() override {
    return std::is_signed_v<T>;
  }

  virtual bool IsFloatingPoint() override {
    return std::is_floating_point_v<T>;
  }

  virtual int UnderlyingBytes() override {
    return sizeof(value);
  }

  virtual double AsDouble() override {
    return value;
  }

  virtual uint64_t AsLongInt() override {
    return value;
  }
};

struct NumberExpression : public AbstractExpression {
  virtual std::unique_ptr<AnyNumber> Get() const = 0;
  virtual CompiledValue Compile(const letsjit::compilation::Context&) const override;
};

}