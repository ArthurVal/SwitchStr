
#include "MatcherMock.hpp"
#include "SwitchStr/SwitchStr.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

TEST(SwitchStrTest, Simple) {
  using swstr::Contains;
  using swstr::SwitchStr;

  EXPECT_EQ(42, SwitchStr<int>("").Default(42));
  EXPECT_EQ(42,
            SwitchStr<int>("Ceci est un string").Case("foo", 0).Default(42));

  std::size_t pos = std::string_view::npos;
  EXPECT_EQ(1, SwitchStr<int>("Ceci est un string")
                   .Case(Contains("foo", &pos), 0)
                   .Case(Contains("est", &pos), 1)
                   .Default(42));
  EXPECT_EQ(5, pos);

  using helper::MatcherMock;
  auto mock = MatcherMock::MakeMockPtr();

  using testing::Return;
  EXPECT_CALL(*mock, IsMatching("foo"))
      .Times(3)
      .WillOnce(Return(false))
      .WillOnce(Return(false))
      .WillOnce(Return(true))
      .RetiresOnSaturation();

  EXPECT_EQ(3, SwitchStr<int>("foo")
                   .Case(MatcherMock(mock), 0)
                   .Case(MatcherMock(mock), 2)
                   .Case(MatcherMock(mock), 3)
                   .Default(42));
}

}  // namespace
