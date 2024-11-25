#pragma once

#include <memory>
#include <string_view>
#include <type_traits>
#include <variant>

namespace swstr {

namespace details {

/**
 *  \brief Meta function use to detect the String Matcher IsMatching() interface
 */
template <typename T, typename = void>
struct HasStrMatcherIsMatchingInterface : std::false_type {};

template <typename T>
struct HasStrMatcherIsMatchingInterface<
    T, std::void_t<decltype(std::declval<T>().IsMatching(std::string_view{}))>>
    : std::is_convertible<
          decltype(std::declval<T>().IsMatching(std::string_view{})), bool> {};

template <typename T>
constexpr bool HasStrMatcherIsMatchingInterface_v =
    HasStrMatcherIsMatchingInterface<T>::value;

/**
 *  \brief Meta function use to detect the String Matcher operator() interface
 */
template <typename T, typename = void>
struct HasStrMatcherOperatorInterface : std::false_type {};

template <typename T>
struct HasStrMatcherOperatorInterface<
    T, std::void_t<decltype(std::declval<T>()(std::string_view{}))>>
    : std::is_convertible<decltype(std::declval<T>()(std::string_view{})),
                          bool> {};

template <typename T>
constexpr bool HasStrMatcherOperatorInterface_v =
    HasStrMatcherOperatorInterface<T>::value;

}  // namespace details

/**
 *  \brief Matcher traits definition
 */
template <typename Matcher>
struct MatcherTraits {
  /// True if the underlying Matcher has the .IsMatching(sv) -> bool interface
  static constexpr bool has_IsMatching =
      details::HasStrMatcherIsMatchingInterface_v<Matcher>;

  /// True if the underlying Matcher has the .operator()(sv) -> bool interface
  static constexpr bool has_Operator =
      details::HasStrMatcherOperatorInterface_v<Matcher>;

  /// True if std::string_view{Matcher{}} is possible
  static constexpr bool is_convertible =
      std::is_convertible_v<Matcher, std::string_view>;

  /// True if one of is_convertible and has_* bool are true
  static constexpr bool is_valid =
      has_IsMatching or has_Operator or is_convertible;

  /**
   *  \brief Perform a static assertion on is_valid with the correct msg
   *
   *  \note This is use to centralized the static_assert msg at one point, where
   *        we define the trait
   */
  static constexpr void StaticAssertIfInvalid() {
    if constexpr (not is_valid) {
      static_assert(
          sizeof(Matcher) == 0,
          "\nThe Matcher type provided doesn't have the correct traits."
          "\nIt must define either:"
          "\n - A 'Matcher{}.IsMatching(std::string_view) -> bool' interface"
          "\n - A 'Matcher{}.operator()(std::string_view) -> bool' interface"
          "\n - A 'std::string_view{Matcher{}}' convertion");
    }
  };
};

/**
 *  \brief Main function use to dispatch the correct function call to the
 *         matcher \a m  with \a str
 *
 *  \tparam Matcher A valid matcher, based on MatcherTraits impl
 *
 *  \param m The matcher
 *  \param str The string to test
 *
 *  \return bool True when the matcher matches, false otherwise
 */
template <typename Matcher>
constexpr auto IsMatching(Matcher&& m, std::string_view str) -> bool {
  using Traits = MatcherTraits<Matcher>;

  if constexpr (Traits::has_IsMatching) {
    return m.IsMatching(str);
  } else if constexpr (Traits::has_Operator) {
    return m(str);
  } else if constexpr (Traits::is_convertible) {
    return str == std::string_view{m};
  } else {
    Traits::StaticAssertIfInvalid();

    // fix warnings
    (void)m;
    (void)str;
    return false;
  }
}

// Simple ///////////////////////////////////////////////////////////////////

/**
 *  \brief Create a matcher that never matches to anything
 */
constexpr auto NeverMatches() {
  return [](std::string_view) noexcept -> bool { return false; };
}

/**
 *  \brief Create a matcher that always matches to everything
 */
constexpr auto AlwaysMatches() {
  return [](std::string_view) noexcept -> bool { return true; };
}

/**
 *  \brief Create a matcher use to check if a string equals \a match
 *
 *  \param[in] match The string we are expecting the match against
 */
constexpr auto Equals(std::string_view match) {
  return
      [match](std::string_view str) noexcept -> bool { return str == match; };
}

/**
 *  \brief Create a matcher use to check if a string STARTS with \a prefix
 *
 *  \param[in] prefix The prefix we are looking for
 */
constexpr auto StartsWith(std::string_view prefix) {
  return [prefix](std::string_view str) noexcept -> bool {
    if (prefix.size() > str.size()) {
      return false;
    } else {
      return str.substr(0, prefix.size()) == prefix;
    }
  };
}

/**
 *  \brief Create a matcher use to check if a string ENDS with \a suffix
 *
 *  \param[in] suffix The suffix we are looking for
 */
constexpr auto EndsWith(std::string_view suffix) {
  return [suffix](std::string_view str) noexcept -> bool {
    if (suffix.size() > str.size()) {
      return false;
    } else {
      return str.substr(str.size() - suffix.size(), suffix.size()) == suffix;
    }
  };
}

// Lookup ///////////////////////////////////////////////////////////////////
namespace details {

template <typename Lookup>
constexpr auto DoLookup(Lookup&& do_lookup,
                        std::variant<std::string_view, char> pattern,
                        std::size_t* const where = nullptr) noexcept -> bool {
  const std::size_t pos = std::visit(std::forward<Lookup>(do_lookup), pattern);
  const bool found = (pos != std::string_view::npos);

  if ((where != nullptr) and found) {
    *where = pos;
  }

  return found;
}

}  // namespace details

/**
 *  \brief Matches when the given \a pattern appears inside the string
 *
 *  \param[in] pattern The char/string pattern we wish to look for
 *  \param[inout] where Set to the index of the start of the FIRST pattern found
 */
constexpr auto Contains(std::variant<std::string_view, char> pattern,
                        std::size_t* const where = nullptr) noexcept {
  return [pattern, where](std::string_view str) noexcept -> bool {
    return details::DoLookup([str](const auto& p) { return str.find(p); },
                             pattern, where);
  };
}

/**
 *  \brief Matches when the given \a pattern appears inside the string
 *
 *  \note Perform reverse lookup
 *
 *  \param[in] pattern The char/string pattern we wish to look for
 *  \param[inout] where Set to the index of the start of the LAST pattern found
 */
constexpr auto ContainsR(std::variant<std::string_view, char> pattern,
                         std::size_t* const where = nullptr) noexcept {
  return [pattern, where](std::string_view str) noexcept -> bool {
    return details::DoLookup([str](const auto& p) { return str.rfind(p); },
                             pattern, where);
  };
}

/**
 *  \brief Matches when ONE OF the character in \a pattern is found inside str
 *
 *  \param[in] pattern The char/string pattern we wish to look for
 *  \param[inout] where Set to the index of the FIRST char found
 */
constexpr auto ContainsOneOf(std::variant<std::string_view, char> pattern,
                             std::size_t* const where = nullptr) noexcept {
  return [pattern, where](std::string_view str) noexcept -> bool {
    return details::DoLookup(
        [str](const auto& p) { return str.find_first_of(p); }, pattern, where);
  };
}

/**
 *  \brief Matches when ONE OF the character in \a pattern is found inside str
 *
 *  \note Perform reverse lookup
 *
 *  \param[in] pattern The char/string pattern we wish to look for
 *  \param[inout] where Set to the index of the LAST char found
 */
constexpr auto ContainsOneOfR(std::variant<std::string_view, char> pattern,
                              std::size_t* const where = nullptr) noexcept {
  return [pattern, where](std::string_view str) noexcept -> bool {
    return details::DoLookup(
        [str](const auto& p) { return str.find_last_of(p); }, pattern, where);
  };
}

// Meta matcher /////////////////////////////////////////////////////////////

/**
 *  \brief Meta matcher returning the negation of the given matcher \a m
 *
 *  \note Matchers must be copyable
 *
 *  \param[in] m Matcher to negate
 */
template <typename Matcher>
constexpr auto DoNot(Matcher&& m) {
  return [=](std::string_view str) -> bool { return not IsMatching(m, str); };
}

/**
 *  \brief Meta matcher returning true if ALL matcher matched the string
 *
 *  \note Matchers are called in the order of declaration
 *  \note Matchers must be copyable
 *
 *  \param[in] ...matchers All matcher to match
 */
template <typename... Matchers>
constexpr auto AllOf(Matchers&&... matchers) {
  return [=](std::string_view str) -> bool {
    return (... and IsMatching(matchers, str));
  };
}

/**
 *  \brief Meta matcher returning true if ONE matcher matched the string
 *
 *  \note Matchers are called in the order of declaration
 *  \note Matchers must be copyable
 *
 *  \param[in] ...matchers All matcher to match
 */
template <typename... Matchers>
constexpr auto AnyOf(Matchers&&... matchers) {
  return [=](std::string_view str) -> bool {
    return (... or IsMatching(matchers, str));
  };
}

// Type erasure /////////////////////////////////////////////////////////////
/**
 *  \brief Type erased matcher use as a runtime polymorphic matcher
 */
class AnyMatcher {
  /// Internal virtual string matcher interface declaration
  struct StrMatcherInterface {
    virtual ~StrMatcherInterface() noexcept = default;
    virtual constexpr auto Clone() const
        -> std::unique_ptr<StrMatcherInterface> = 0;
    virtual constexpr auto IsMatching(std::string_view) -> bool = 0;
  };

  /// String matcher templated wrapper, defining the StrMatcherInterface for any
  /// \a InnerMatcher
  template <typename InnerMatcher>
  struct StrMatcherWrapper final : StrMatcherInterface {
    /**
     *  \brief Construct the Wrapper inplace, forwarding \a args to the
     *         underlying InnerMatcher constructor
     */
    template <typename... Args>
    constexpr StrMatcherWrapper(Args&&... args)
        : m_matcher(std::forward<Args>(args)...) {}

    /**
     *  \brief Clone the current Wrapper
     *
     *  \return An unique copy of the current Wrapper, as StrMatcherInterface
     */
    inline auto Clone() const -> std::unique_ptr<StrMatcherInterface> override {
      return std::make_unique<StrMatcherWrapper>(*this);
    }

    /**
     *  \brief Implement the IsMatching interface using IsMatching() function
     *
     *  \param[in] str The string we wish to matches against
     *
     *  \return True when the matches succeed, false otherwise
     */
    constexpr auto IsMatching(std::string_view str) -> bool override {
      // The full path namespace is used in order to desambiguate between the
      // function and the method
      return ::swstr::IsMatching(m_matcher, str);
    }

   private:
    InnerMatcher m_matcher; /*!< Inner matcher wrapped */
  };

 public:
  /**
   *  \brief Construct a new AnyMatcher from any other matcher type
   *
   *  \note If using an lvalue Matcher, Matcher must be copy constructible
   *
   *  \param[in] m The matcher we wish to wrap
   */
  template <typename Matcher,
            std::enable_if_t<
                not std::is_same_v<AnyMatcher, std::remove_cvref_t<Matcher>>,
                bool> = true>
  constexpr explicit AnyMatcher(Matcher&& m)
      : m_interface(std::make_unique<StrMatcherWrapper<Matcher>>(
            std::forward<Matcher>(m))) {
    MatcherTraits<Matcher>::StaticAssertIfInvalid();
  }

  /**
   *  \brief Default construct AnyMatcher using NeverMatches()
   */
  constexpr AnyMatcher() noexcept : AnyMatcher(NeverMatches()) {}

  /**
   *  \brief Copy construct AnyMatcher cloning the \a other wrapped matcher
   *
   *  \param[in] other An lvalue AnyMatcher we wish to copy
   */
  inline explicit AnyMatcher(const AnyMatcher& other)
      : m_interface(other.m_interface->Clone()) {}

  /**
   *  \brief Copy assign cloning the \a other wrapped matcher
   *
   *  \param[in] other An lvalue AnyMatcher we wish to copy
   */
  inline AnyMatcher& operator=(const AnyMatcher& other) {
    m_interface = other.m_interface->Clone();
    return *this;
  }

  /**
   *  \brief Move construct AnyMatcher using \a other wrapped matcher
   *
   *  \param[in] other An rvalue AnyMatcher we are constructing from
   */
  inline explicit AnyMatcher(AnyMatcher&& other) noexcept
      : m_interface(std::move(other.m_interface)) {}

  /**
   *  \brief Move assign AnyMatcher using \a other wrapped matcher
   *
   *  \param[in] other An rvalue AnyMatcher
   */
  inline AnyMatcher& operator=(AnyMatcher&& other) noexcept {
    m_interface = std::move(other.m_interface);
    return *this;
  }

  /// Default destructor
  virtual ~AnyMatcher() noexcept = default;

  /**
   *  \brief Assigns a Matcher to the AnyMatcher
   *
   *  \note If using an lvalue Matcher, Matcher must be copy constructible
   *
   *  \param[in] m The matcher we wish to wrap
   */
  template <
      typename Matcher,
      std::enable_if_t<not std::is_same_v<AnyMatcher, std::decay_t<Matcher>>,
                       bool> = true>
  constexpr AnyMatcher& operator=(Matcher&& m) {
    *this = AnyMatcher(std::forward<Matcher>(m));
    return *this;
  }

  /**
   *  \brief Default matcher interface forwaring the call to the underlying
   *         matcher
   *
   *  \param[in] str The string to matches against
   *
   *  \return True when the underlying matcher matched, false otherwise
   */
  inline auto IsMatching(std::string_view str) const -> bool {
    return m_interface->IsMatching(str);
  }

 private:
  std::unique_ptr<StrMatcherInterface>
      m_interface; /*!< Generic wrapper pointer */
};

}  // namespace swstr
