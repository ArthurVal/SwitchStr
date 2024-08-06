#pragma once

#include <optional>

#include "SwitchStr/Matcher.hpp"

namespace swstr {

// TODO: Supprimer ResulType et utiliser des Actions au lieux de retourner une
// valeur ?
// Example:
//
// SwitchStr("Toto")
//  .Case("foo", Invoke(MyFunction, a, b, c))
//  .Case("Toto", Set(lvalue, 2))
//  ...

/**
 *  \brief Construct use to perform a Switch of a string
 *
 *  \tparam ResultType The type used and returned by the switch
 */
template <typename ResultType>
struct SwitchStr {
  constexpr SwitchStr() = delete;
  constexpr SwitchStr(std::string_view str) : m_str(str), m_res(std::nullopt){};

  template <typename T = ResultType>
  constexpr operator T() const noexcept {
    static_assert(sizeof(T) == 0, "Missing a Default() switch statement.");
    return std::move(*m_res);
  }

  template <typename T>
  constexpr decltype(auto) Default(T&& val) const noexcept {
    return m_res.value_or(std::forward<T>(val));
  }

  template <typename Matcher>
  constexpr SwitchStr& Case(Matcher&& m, ResultType value) {
    if (not m_res.has_value() and IsMatching(std::forward<Matcher>(m), m_str)) {
      m_res = std::move(value);
    }

    return *this;
  }

 private:
  std::string_view m_str;
  std::optional<ResultType> m_res;
};

}  // namespace swstr
