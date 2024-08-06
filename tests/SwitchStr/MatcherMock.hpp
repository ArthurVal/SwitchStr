#pragma once

#include <memory>

#include "SwitchStr/Matcher.hpp"
#include "gmock/gmock.h"

namespace helper {

struct MatcherMock {
  struct Mock {
    MOCK_METHOD(bool, IsMatching, (std::string_view), (const));
  };

  static inline auto MakeMockPtr() -> std::shared_ptr<Mock> {
    return std::make_shared<Mock>();
  }

  inline MatcherMock() : MatcherMock(MakeMockPtr()) {}

  inline explicit MatcherMock(std::shared_ptr<Mock> mock) : m_mock(mock) {}

  inline auto ShareMock() const -> std::shared_ptr<Mock> { return m_mock; }

  inline auto GetMock() -> Mock& { return *m_mock; }
  inline auto GetMock() const -> const Mock& { return *m_mock; }

  inline auto IsMatching(std::string_view str) const -> bool {
    return m_mock->IsMatching(str);
  }

 private:
  std::shared_ptr<Mock> m_mock;
};

}  // namespace helper
