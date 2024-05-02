// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2024 Second State INC
#pragma once

#include "common/enum_types.hpp"
#include "common/errcode.h"
#include "common/int128.h"
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
  String,
  List
};

class InterfaceType {
public:
  InterfaceType() {}
  InterfaceType(ITypeCode C) : Code{C}, TyArgs{} {}
  InterfaceType(ITypeCode C, InterfaceType &&T)
      : Code{C}, TyArgs{std::move(T)} {}

  const ITypeCode &getCode() const noexcept { return Code; }
  Span<const InterfaceType> getTypeArgs() const noexcept { return TyArgs; }

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
  InterfaceValue(std::string Init)
      : Val(Init), Ty(convertType<std::string>::type) {}

  template <typename T> const T &get() const { std::get<T>(Val); }
  template <typename T> T &get() { std::get<T>(Val); }

private:
  std::variant<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t,
               int64_t, float, double, std::string, ValVariant>
      Val;
  InterfaceType Ty;
};

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
    using namespace WasmEdge::AST::Component;
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
