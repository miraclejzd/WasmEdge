// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2024 Second State INC
#pragma once

#include "common/enum_types.hpp"
#include "common/errcode.h"
#include "common/int128.h"
#include "common/types.h"
#include "common/variant.h"

#include <array>
#include <cstdint>
#include <type_traits>
#include <variant>

namespace WasmEdge {

using Byte = uint8_t;

enum class ITypeCode : Byte {
  I8,
  I16,
  I32,
  I64,
  U8,
  U16,
  U32,
  U64,
  F32,
  F64,
  V128,
  String,
  List
};

class InterfaceType {
public:
  InterfaceType() {}
  InterfaceType(ITypeCode C) : Code{C}, TyArgs{} {}
  InterfaceType(ITypeCode C, InterfaceType &&T)
      : Code{C}, TyArgs{std::move(T)} {}
  InterfaceType(const ValType &T) : TyArgs{} {
    switch (T.getCode()) {
    case TypeCode::I8:
      Code = ITypeCode::I8;
      break;
    case TypeCode::I16:
      Code = ITypeCode::I16;
      break;
    case TypeCode::I32:
      Code = ITypeCode::I32;
      break;
    case TypeCode::I64:
      Code = ITypeCode::I64;
      break;
    case TypeCode::F32:
      Code = ITypeCode::F32;
      break;
    case TypeCode::F64:
      Code = ITypeCode::F64;
      break;
    case TypeCode::V128:
      Code = ITypeCode::V128;
      break;
    default:
      throw 1;
    }
  }

  const ITypeCode &getCode() const noexcept { return Code; }
  Span<const InterfaceType> getTypeArgs() const noexcept { return TyArgs; }

  ValType toValType() const {
    switch (Code) {
    case ITypeCode::I8:
      return TypeCode::I8;
    case ITypeCode::I16:
      return TypeCode::I16;
    case ITypeCode::I32:
      return TypeCode::I32;
    case ITypeCode::I64:
      return TypeCode::I64;
    case ITypeCode::U8:
      return TypeCode::I8;
    case ITypeCode::U16:
      return TypeCode::I16;
    case ITypeCode::U32:
      return TypeCode::I32;
    case ITypeCode::U64:
      return TypeCode::I64;
    case ITypeCode::F32:
      return TypeCode::F32;
    case ITypeCode::F64:
      return TypeCode::F64;
    case ITypeCode::V128:
      return TypeCode::V128;
    default:
      throw 1;
    }
  }

private:
  ITypeCode Code;
  std::vector<InterfaceType> TyArgs;
};

template <typename T> struct convertType {
  inline static const InterfaceType type;
};

template <> struct convertType<uint8_t> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::U8);
};
template <> struct convertType<uint16_t> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::U16);
};
template <> struct convertType<uint32_t> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::U32);
};
template <> struct convertType<uint64_t> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::U64);
};
template <> struct convertType<int8_t> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::I8);
};
template <> struct convertType<int16_t> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::I16);
};
template <> struct convertType<int32_t> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::I32);
};
template <> struct convertType<int64_t> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::I64);
};
template <> struct convertType<float> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::F32);
};
template <> struct convertType<double> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::F64);
};
template <> struct convertType<uint128_t> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::V128);
};
template <> struct convertType<std::string> {
  inline static const InterfaceType type = InterfaceType(ITypeCode::String);
};

template <typename T> struct convertType<std::vector<T>> {
  inline static const InterfaceType type =
      InterfaceType(ITypeCode::List, convertType<T>::type);
};

class InterfaceValue {
public:
  InterfaceValue() {}

  InterfaceValue(uint8_t Init) : Val(Init), Ty(convertType<uint8_t>::type) {}
  InterfaceValue(uint16_t Init) : Val(Init), Ty(convertType<uint16_t>::type) {}
  InterfaceValue(uint32_t Init) : Val(Init), Ty(convertType<uint32_t>::type) {}
  InterfaceValue(uint64_t Init) : Val(Init), Ty(convertType<uint64_t>::type) {}
  InterfaceValue(int8_t Init) : Val(Init), Ty(convertType<int8_t>::type) {}
  InterfaceValue(int16_t Init) : Val(Init), Ty(convertType<int16_t>::type) {}
  InterfaceValue(int32_t Init) : Val(Init), Ty(convertType<int32_t>::type) {}
  InterfaceValue(int64_t Init) : Val(Init), Ty(convertType<int64_t>::type) {}
  InterfaceValue(float Init) : Val(Init), Ty(convertType<float>::type) {}
  InterfaceValue(double Init) : Val(Init), Ty(convertType<double>::type) {}
  InterfaceValue(int128_t Init) : Val(Init), Ty(convertType<int128_t>::type) {}
  InterfaceValue(std::string &&Init)
      : Val(Init), Ty(convertType<std::string>::type) {}

  struct VisitVal {
    ValVariant operator()(uint8_t V) const { return static_cast<uint32_t>(V); }
    ValVariant operator()(uint16_t V) const { return static_cast<uint32_t>(V); }
    ValVariant operator()(uint32_t V) const { return V; }
    ValVariant operator()(uint64_t V) const { return V; }
    ValVariant operator()(int8_t V) const { return static_cast<int32_t>(V); }
    ValVariant operator()(int16_t V) const { return static_cast<int32_t>(V); }
    ValVariant operator()(int32_t V) const { return V; }
    ValVariant operator()(int64_t V) const { return V; }
    ValVariant operator()(int128_t V) const { return V; }
    ValVariant operator()(float V) const { return V; }
    ValVariant operator()(double V) const { return V; }
    // Some cases should fail.
    ValVariant operator()(std::string) { throw 1; }
  };
  ValVariant toValVariant() const { return std::visit(VisitVal(), Val); }

  template <typename T> const T &get() const noexcept {
    return std::get<T>(Val);
  }
  template <typename T> T &get() noexcept { return std::get<T>(Val); }

private:
  std::variant<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t,
               int64_t, int128_t, float, double, std::string>
      Val;
  InterfaceType Ty;
};

InterfaceValue liftValue(const ValType &T, ValVariant &V) {
  switch (T.getCode()) {
  case TypeCode::I8:
    return InterfaceValue(static_cast<int8_t>(V.get<int32_t>()));
  case TypeCode::I16:
    return InterfaceValue(static_cast<int16_t>(V.get<int32_t>()));
  case TypeCode::I32:
    return InterfaceValue(V.get<int32_t>());
  case TypeCode::I64:
    return InterfaceValue(V.get<int64_t>());
  case TypeCode::V128:
    return InterfaceValue(V.get<int128_t>());
  case TypeCode::F32:
    return InterfaceValue(V.get<float>());
  case TypeCode::F64:
    return InterfaceValue(V.get<double>());
  default:
    throw 1;
  }
}

InterfaceValue liftValue(const InterfaceType &T, ValVariant &V) {
  switch (T.getCode()) {
  case ITypeCode::I8:
    return InterfaceValue(static_cast<int8_t>(V.get<int32_t>()));
  case ITypeCode::I16:
    return InterfaceValue(static_cast<int16_t>(V.get<int32_t>()));
  case ITypeCode::I32:
    return InterfaceValue(V.get<int32_t>());
  case ITypeCode::I64:
    return InterfaceValue(V.get<int64_t>());
  case ITypeCode::V128:
    return InterfaceValue(V.get<int128_t>());
  case ITypeCode::U8:
    return InterfaceValue(static_cast<uint8_t>(V.get<int32_t>()));
  case ITypeCode::U16:
    return InterfaceValue(static_cast<uint16_t>(V.get<int32_t>()));
  case ITypeCode::U32:
    return InterfaceValue(V.get<uint32_t>());
  case ITypeCode::U64:
    return InterfaceValue(V.get<uint64_t>());
  case ITypeCode::F32:
    return InterfaceValue(V.get<float>());
  case ITypeCode::F64:
    return InterfaceValue(V.get<double>());
  default:
    throw 1;
  }
}

class FunctionType {
public:
  FunctionType() : ParamList{}, ResList{} {}
  FunctionType(std::vector<InterfaceType> P, std::vector<InterfaceType> R)
      : ParamList{P}, ResList{R} {}

  Span<const InterfaceType> getParamTypes() const noexcept { return ParamList; }
  std::vector<InterfaceType> &getParamTypes() noexcept { return ParamList; }
  Span<const InterfaceType> getReturnTypes() const noexcept { return ResList; }
  std::vector<InterfaceType> &getReturnTypes() noexcept { return ResList; }

private:
  std::vector<InterfaceType> ParamList;
  std::vector<InterfaceType> ResList;
};

} // namespace WasmEdge

template <>
struct fmt::formatter<WasmEdge::InterfaceType>
    : fmt::formatter<std::string_view> {
  fmt::format_context::iterator
  format(const WasmEdge::InterfaceType &Type,
         fmt::format_context &Ctx) const noexcept {
    using namespace WasmEdge;
    using namespace std::literals;
    fmt::memory_buffer Buffer;

    switch (Type.getCode()) {
    case ITypeCode::I8:
      fmt::format_to(std::back_inserter(Buffer), "s8"sv);
      break;
    case ITypeCode::I16:
      fmt::format_to(std::back_inserter(Buffer), "s16"sv);
      break;
    case ITypeCode::I32:
      fmt::format_to(std::back_inserter(Buffer), "s32"sv);
      break;
    case ITypeCode::I64:
      fmt::format_to(std::back_inserter(Buffer), "s64"sv);
      break;
    case ITypeCode::U8:
      fmt::format_to(std::back_inserter(Buffer), "u8"sv);
      break;
    case ITypeCode::U16:
      fmt::format_to(std::back_inserter(Buffer), "u16"sv);
      break;
    case ITypeCode::U32:
      fmt::format_to(std::back_inserter(Buffer), "u32"sv);
      break;
    case ITypeCode::U64:
      fmt::format_to(std::back_inserter(Buffer), "u64"sv);
      break;
    case ITypeCode::F32:
      fmt::format_to(std::back_inserter(Buffer), "f32"sv);
      break;
    case ITypeCode::F64:
      fmt::format_to(std::back_inserter(Buffer), "f64"sv);
      break;
    case ITypeCode::List:
      fmt::format_to(std::back_inserter(Buffer), "list<"sv);
      for (const auto &Ty : Type.getTypeArgs()) {
        fmt::format_to(std::back_inserter(Buffer), "{}"sv, Ty);
      }
      fmt::format_to(std::back_inserter(Buffer), "list>"sv);
    default:
      break;
    }

    return formatter<std::string_view>::format(
        std::string_view(Buffer.data(), Buffer.size()), Ctx);
  }
};

template <>
struct fmt::formatter<WasmEdge::FunctionType>
    : fmt::formatter<std::string_view> {
  fmt::format_context::iterator
  format(const WasmEdge::FunctionType &Type,
         fmt::format_context &Ctx) const noexcept {
    using namespace std::literals;

    fmt::memory_buffer Buffer;

    fmt::format_to(std::back_inserter(Buffer), "[ "sv);
    for (const auto &Ty : Type.getParamTypes()) {
      fmt::format_to(std::back_inserter(Buffer), "{} "sv, Ty);
    }
    fmt::format_to(std::back_inserter(Buffer), "] -> "sv);

    fmt::format_to(std::back_inserter(Buffer), "[ "sv);
    for (const auto &Ty : Type.getReturnTypes()) {
      fmt::format_to(std::back_inserter(Buffer), "{} "sv, Ty);
    }
    fmt::format_to(std::back_inserter(Buffer), "]"sv);

    return formatter<std::string_view>::format(
        std::string_view(Buffer.data(), Buffer.size()), Ctx);
  }
};
