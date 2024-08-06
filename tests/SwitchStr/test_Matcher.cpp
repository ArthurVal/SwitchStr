#include "MatcherMock.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

TEST(SwitchStrMatcherTest, IsMatching) {
  using swstr::IsMatching;

  EXPECT_TRUE(IsMatching("foo", "foo"));
  EXPECT_FALSE(IsMatching("foo", "bar"));

  using helper::MatcherMock;
  auto matcher = MatcherMock();

  using testing::Return;
  EXPECT_CALL(matcher.GetMock(), IsMatching("foo"))
      .Times(2)
      .WillOnce(Return(true))
      .WillOnce(Return(false))
      .RetiresOnSaturation();

  EXPECT_TRUE(IsMatching(matcher, "foo"));
  EXPECT_FALSE(IsMatching(matcher, "foo"));
}

TEST(SwitchStrMatcherTest, Equals) {
  using swstr::Equals;

  EXPECT_TRUE(IsMatching(Equals("Toto"), "Toto"));
  EXPECT_FALSE(IsMatching(Equals("Toto"), ""));
  EXPECT_FALSE(IsMatching(Equals("Toto"), "Tot"));
  EXPECT_FALSE(IsMatching(Equals("Toto"), "oto"));
  EXPECT_FALSE(IsMatching(Equals("Toto"), "Tototo"));
  EXPECT_FALSE(IsMatching(Equals("Toto"), "foo"));
}

TEST(SwitchStrMatcherTest, StartsWith) {
  using swstr::StartsWith;

  EXPECT_TRUE(IsMatching(StartsWith("foo"), "foo"));
  EXPECT_TRUE(IsMatching(StartsWith("foo"), "foobarbaz"));
  EXPECT_TRUE(IsMatching(StartsWith("foo"), "foo barbaz"));
  EXPECT_FALSE(IsMatching(StartsWith("foo"), "bar"));
  EXPECT_FALSE(IsMatching(StartsWith("foo"), ""));
  EXPECT_FALSE(IsMatching(StartsWith("foo"), "bar foo"));
  EXPECT_FALSE(IsMatching(StartsWith("foo"), "bar foo baz"));
  EXPECT_FALSE(IsMatching(StartsWith("foo"), " foo baz"));
}

TEST(SwitchStrMatcherTest, EndsWith) {
  using swstr::EndsWith;

  EXPECT_TRUE(IsMatching(EndsWith("foo"), "foo"));
  EXPECT_TRUE(IsMatching(EndsWith("foo"), "barbazfoo"));
  EXPECT_TRUE(IsMatching(EndsWith("foo"), "barbaz foo"));
  EXPECT_FALSE(IsMatching(EndsWith("foo"), "bar"));
  EXPECT_FALSE(IsMatching(EndsWith("foo"), ""));
  EXPECT_FALSE(IsMatching(EndsWith("foo"), "foo bar"));
  EXPECT_FALSE(IsMatching(EndsWith("foo"), "bar foo baz"));
  EXPECT_FALSE(IsMatching(EndsWith("foo"), "bar foo "));
}

TEST(SwitchStrMatcherTest, Contains) {
  using swstr::Contains;

  std::size_t pos = std::string_view::npos;
  EXPECT_TRUE(IsMatching(Contains("foo"), "foo"));

  EXPECT_TRUE(IsMatching(Contains("foo", &pos), "foo"));
  EXPECT_EQ(pos, 0);

  EXPECT_TRUE(IsMatching(Contains("foo", &pos), "0123foo43210"));
  EXPECT_EQ(pos, 4);

  EXPECT_TRUE(IsMatching(Contains("foo", &pos), "foofoofoo"));
  EXPECT_EQ(pos, 0);

  pos = std::string_view::npos;
  EXPECT_FALSE(IsMatching(Contains("foo"), "bar"));

  EXPECT_FALSE(IsMatching(Contains("foo", &pos), "bar"));
  EXPECT_EQ(pos, std::string_view::npos);

  EXPECT_FALSE(IsMatching(Contains("foo"), "fo o"));
  EXPECT_FALSE(IsMatching(Contains("foo"), "oof"));
  EXPECT_FALSE(IsMatching(Contains("foo"), "oo"));
  EXPECT_FALSE(IsMatching(Contains("foo"), ""));
  EXPECT_FALSE(IsMatching(Contains("foo"), "bar baz"));
  EXPECT_FALSE(IsMatching(Contains("foo"), "bar fo "));
}

TEST(SwitchStrMatcherTest, ContainsR) {
  using swstr::ContainsR;

  std::size_t pos = std::string_view::npos;
  EXPECT_TRUE(IsMatching(ContainsR("foo"), "foo"));

  EXPECT_TRUE(IsMatching(ContainsR("foo", &pos), "foo"));
  EXPECT_EQ(pos, 0);

  EXPECT_TRUE(IsMatching(ContainsR("foo", &pos), "0123foo43210"));
  EXPECT_EQ(pos, 4);

  EXPECT_TRUE(IsMatching(ContainsR("foo", &pos), "foofoofoo"));
  EXPECT_EQ(pos, 6);

  pos = std::string_view::npos;
  EXPECT_FALSE(IsMatching(ContainsR("foo"), "bar"));

  EXPECT_FALSE(IsMatching(ContainsR("foo", &pos), "bar"));
  EXPECT_EQ(pos, std::string_view::npos);

  EXPECT_FALSE(IsMatching(ContainsR("foo"), "fo o"));
  EXPECT_FALSE(IsMatching(ContainsR("foo"), "oof"));
  EXPECT_FALSE(IsMatching(ContainsR("foo"), "oo"));
  EXPECT_FALSE(IsMatching(ContainsR("foo"), ""));
  EXPECT_FALSE(IsMatching(ContainsR("foo"), "bar baz"));
  EXPECT_FALSE(IsMatching(ContainsR("foo"), "bar fo "));
}

TEST(SwitchStrMatcherTest, ContainsOneOf) {
  using swstr::ContainsOneOf;

  std::size_t pos = std::string_view::npos;
  EXPECT_TRUE(IsMatching(ContainsOneOf("foo"), "foo"));

  EXPECT_TRUE(IsMatching(ContainsOneOf("foo", &pos), "foo"));
  EXPECT_EQ(pos, 0);

  EXPECT_TRUE(IsMatching(ContainsOneOf("foo", &pos), "0123foo43210"));
  EXPECT_EQ(pos, 4);

  EXPECT_TRUE(IsMatching(ContainsOneOf("foo", &pos), "foofoofoo"));
  EXPECT_EQ(pos, 0);

  pos = std::string_view::npos;
  EXPECT_FALSE(IsMatching(ContainsOneOf("foo"), "bar"));

  EXPECT_FALSE(IsMatching(ContainsOneOf("foo", &pos), "bar"));
  EXPECT_EQ(pos, std::string_view::npos);

  EXPECT_TRUE(IsMatching(ContainsOneOf("foo"), "fo o"));
  EXPECT_TRUE(IsMatching(ContainsOneOf("foo"), "oof"));
  EXPECT_TRUE(IsMatching(ContainsOneOf("foo"), "oo"));
  EXPECT_FALSE(IsMatching(ContainsOneOf("foo"), ""));
  EXPECT_FALSE(IsMatching(ContainsOneOf("foo"), "bar baz"));
  EXPECT_TRUE(IsMatching(ContainsOneOf("foo"), "bar fo "));
}

TEST(SwitchStrMatcherTest, ContainsOneOfR) {
  using swstr::ContainsOneOfR;

  std::size_t pos = std::string_view::npos;
  EXPECT_TRUE(IsMatching(ContainsOneOfR("foo"), "foo"));

  EXPECT_TRUE(IsMatching(ContainsOneOfR("foo", &pos), "foo"));
  EXPECT_EQ(pos, 2);

  EXPECT_TRUE(IsMatching(ContainsOneOfR("foo", &pos), "0123foo43210"));
  EXPECT_EQ(pos, 6);

  EXPECT_TRUE(IsMatching(ContainsOneOfR("foo", &pos), "foofoofoo"));
  EXPECT_EQ(pos, 8);

  pos = std::string_view::npos;
  EXPECT_FALSE(IsMatching(ContainsOneOfR("foo"), "bar"));

  EXPECT_FALSE(IsMatching(ContainsOneOfR("foo", &pos), "bar"));
  EXPECT_EQ(pos, std::string_view::npos);

  EXPECT_TRUE(IsMatching(ContainsOneOfR("foo"), "fo o"));
  EXPECT_TRUE(IsMatching(ContainsOneOfR("foo"), "oof"));
  EXPECT_TRUE(IsMatching(ContainsOneOfR("foo"), "oo"));
  EXPECT_FALSE(IsMatching(ContainsOneOfR("foo"), ""));
  EXPECT_FALSE(IsMatching(ContainsOneOfR("foo"), "bar baz"));
  EXPECT_TRUE(IsMatching(ContainsOneOfR("foo"), "bar fo "));
}

TEST(SwitchStrMatcherTest, DoNot) {
  using swstr::DoNot;
  using swstr::Equals;

  EXPECT_FALSE(IsMatching(DoNot("foo"), "foo"));
  EXPECT_TRUE(IsMatching(DoNot("foo"), ""));
  EXPECT_TRUE(IsMatching(DoNot("foo"), "bar"));
  EXPECT_TRUE(IsMatching(DoNot(Equals("foo")), ""));

  using helper::MatcherMock;
  auto matcher = MatcherMock();

  using testing::Return;
  EXPECT_CALL(matcher.GetMock(), IsMatching("foo"))
      .Times(2)
      .WillOnce(Return(true))
      .WillOnce(Return(false))
      .RetiresOnSaturation();

  EXPECT_FALSE(IsMatching(DoNot(matcher), "foo"));
  EXPECT_TRUE(IsMatching(DoNot(matcher), "foo"));
}

TEST(SwitchStrMatcherTest, AllOf) {
  using swstr::AllOf;
  using swstr::DoNot;

  EXPECT_TRUE(IsMatching(AllOf("foo", DoNot("bar"), DoNot("baz")), "foo"));
  EXPECT_FALSE(IsMatching(AllOf("fo", DoNot("bar"), DoNot("baz")), "foo"));
  EXPECT_FALSE(IsMatching(AllOf("foo", DoNot("foo"), DoNot("baz")), "foo"));

  using helper::MatcherMock;
  auto matcher_1 = MatcherMock();
  auto matcher_2 = MatcherMock();

  using testing::Return;

  {
    ::testing::InSequence seq;

    EXPECT_CALL(matcher_1.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_CALL(matcher_2.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_FALSE(IsMatching(AllOf(matcher_1, matcher_2), "foo"));
  }

  {
    ::testing::InSequence seq;

    EXPECT_CALL(matcher_1.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_FALSE(IsMatching(AllOf(matcher_1, matcher_2), "foo"));
  }

  {
    ::testing::InSequence seq;

    EXPECT_CALL(matcher_1.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_CALL(matcher_2.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_TRUE(IsMatching(AllOf(matcher_1, matcher_2), "foo"));
  }

  {
    ::testing::InSequence seq;

    EXPECT_CALL(matcher_2.GetMock(), IsMatching("bar"))
        .Times(1)
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_CALL(matcher_1.GetMock(), IsMatching("bar"))
        .Times(1)
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_TRUE(IsMatching(AllOf(matcher_2, matcher_1), "bar"));
  }
}

TEST(SwitchStrMatcherTest, AnyOf) {
  using swstr::AnyOf;
  using swstr::DoNot;

  EXPECT_TRUE(IsMatching(AnyOf("foo", DoNot("bar"), DoNot("baz")), "foo"));
  EXPECT_FALSE(IsMatching(AnyOf("fo", DoNot("foo"), "bar"), "foo"));
  EXPECT_TRUE(IsMatching(AnyOf("foo", DoNot("foo"), DoNot("baz")), "foo"));

  using helper::MatcherMock;
  auto matcher_1 = MatcherMock();
  auto matcher_2 = MatcherMock();

  using testing::Return;

  {
    ::testing::InSequence seq;

    EXPECT_CALL(matcher_1.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_TRUE(IsMatching(AnyOf(matcher_1, matcher_2), "foo"));
  }

  {
    ::testing::InSequence seq;

    EXPECT_CALL(matcher_1.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_CALL(matcher_2.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_TRUE(IsMatching(AnyOf(matcher_1, matcher_2), "foo"));
  }

  {
    ::testing::InSequence seq;

    EXPECT_CALL(matcher_1.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_CALL(matcher_2.GetMock(), IsMatching("foo"))
        .Times(1)
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_FALSE(IsMatching(AnyOf(matcher_1, matcher_2), "foo"));
  }

  {
    ::testing::InSequence seq;

    EXPECT_CALL(matcher_2.GetMock(), IsMatching("bar"))
        .Times(1)
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_CALL(matcher_1.GetMock(), IsMatching("bar"))
        .Times(1)
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_FALSE(IsMatching(AnyOf(matcher_2, matcher_1), "bar"));
  }
}

#define TEST_ANY_MATCHER(any, matcher)              \
  do {                                              \
    SCOPED_TRACE("\nTesting " #any " = " #matcher); \
                                                    \
    any = (matcher);                                \
    for (auto str : {"foo", "bar", "baz", ""}) {    \
      EXPECT_EQ(swstr::IsMatching((any), str),      \
                swstr::IsMatching((matcher), str))  \
          << "str = \"" << str << "\"";             \
    }                                               \
                                                    \
  } while (0)

TEST(SwitchStrMatcherTest, AnyMatcher) {
  using swstr::AnyMatcher;
  using swstr::Equals;
  // using DoNot not necessary thanks to ADL

  auto any_matcher = AnyMatcher();
  TEST_ANY_MATCHER(any_matcher, Equals("foo"));
  TEST_ANY_MATCHER(any_matcher, "bar");
  TEST_ANY_MATCHER(any_matcher, DoNot(Equals("foo")));

  // any_matcher = 25; // Do not compile

  auto l_value_equals = Equals("baz");
  TEST_ANY_MATCHER(any_matcher, l_value_equals);

  const auto const_l_value_matcher = Equals("baz");
  TEST_ANY_MATCHER(any_matcher, const_l_value_matcher);

  using helper::MatcherMock;
  using testing::Return;
  auto mock_ptr = MatcherMock::MakeMockPtr();
  EXPECT_CALL(*mock_ptr, IsMatching("foo"))
      .Times(1)
      .WillOnce(Return(true))
      .RetiresOnSaturation();

  any_matcher = MatcherMock(mock_ptr);
  EXPECT_TRUE(IsMatching(any_matcher, "foo"));

  auto lvalue_mock = MatcherMock(mock_ptr);
  EXPECT_CALL(lvalue_mock.GetMock(), IsMatching("bar"))
      .Times(1)
      .WillOnce(Return(false))
      .RetiresOnSaturation();

  any_matcher = lvalue_mock;
  EXPECT_FALSE(IsMatching(any_matcher, "bar"));
}

}  // namespace
