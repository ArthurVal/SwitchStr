#include <concepts>
#include <iostream>
#include <optional>
#include <string_view>
#include <variant>

template <template <typename...> typename, typename>
struct IsSpecializationOf : std::false_type {};

template <template <typename...> typename T, typename... Args>
struct IsSpecializationOf<T, T<Args...>> : std::true_type {};

template <template <typename...> typename U, typename T>
constexpr bool IsSpecializationOf_v = IsSpecializationOf<U, T>::value;

template <typename T>
concept VariantType =
    IsSpecializationOf_v<std::variant, std::remove_cvref_t<T>>;

template <typename T>
concept StrMatcherInterface = requires(T matcher) {
    { matcher(std::string_view{}) } -> std::convertible_to<bool>;
};

template <typename T>
concept StringLike = std::convertible_to<T, std::string_view>;

template <typename T>
concept StrMatcher = StrMatcherInterface<T> or StringLike<T>;

template <StrMatcher Matcher>
constexpr bool IsMatching(Matcher&& match, std::string_view str) {
    if constexpr (StringLike<Matcher>) {
        return str.compare(std::forward<Matcher>(match)) == 0;
    } else {
        return match(str);
    }
}

constexpr auto Equals(std::string_view match) {
    return [match](std::string_view str) { return str.compare(match) == 0; };
}

constexpr auto StartsWith(std::string_view match) {
    return [match](std::string_view str) { return str.starts_with(match); };
}

constexpr auto EndsWith(std::string_view match) {
    return [match](std::string_view str) { return str.ends_with(match); };
}

namespace details {

template <VariantType V, typename F>
constexpr auto ContainsImpl(
    V&& v, F&& f, std::size_t* const where = nullptr) noexcept -> bool {
    const std::size_t pos = std::visit(std::forward<F>(f), std::forward<V>(v));
    const bool found = (pos != std::string_view::npos);
    if ((where != nullptr) and found) {
        *where = pos;
    }

    return found;
}

}  // namespace details

constexpr auto Contains(std::variant<char, std::string_view> pattern,
                        std::size_t* const where = nullptr) {
    return [pattern, where](std::string_view str) noexcept -> bool {
        return details::ContainsImpl(
            pattern, [str](const auto& val) { return str.find(val); }, where);
    };
}

constexpr auto ContainsR(std::variant<char, std::string_view> pattern,
                         std::size_t* const where = nullptr) {
    return [pattern, where](std::string_view str) noexcept -> bool {
        return details::ContainsImpl(
            pattern, [str](const auto& val) { return str.rfind(val); }, where);
    };
}

constexpr auto ContainsOneOf(std::variant<char, std::string_view> pattern,
                             std::size_t* const where = nullptr) {
    return [pattern, where](std::string_view str) noexcept -> bool {
        return details::ContainsImpl(
            pattern, [str](const auto& val) { return str.find_first_of(val); },
            where);
    };
}

constexpr auto ContainsOneOfR(std::variant<char, std::string_view> pattern,
                              std::size_t* const where = nullptr) {
    return [pattern, where](std::string_view str) noexcept -> bool {
        return details::ContainsImpl(
            pattern, [str](const auto& val) { return str.find_last_of(val); },
            where);
    };
}

template <StrMatcher Matcher>
constexpr auto DoNot(Matcher&& matcher) {
    return [m = std::forward<Matcher>(matcher)](std::string_view str) {
        return not IsMatching(m, str);
    };
}

template <StrMatcher... Matchers>
constexpr auto AnyOf(Matchers&&... matchers) {
    static_assert(sizeof...(Matchers) > 0);
    return [... m = std::forward<Matchers>(matchers)](std::string_view str) {
        return (... or IsMatching(m, str));
    };
}

template <StrMatcher... Matchers>
constexpr auto AllOf(Matchers&&... matchers) {
    static_assert(sizeof...(Matchers) > 0);
    return [... m = std::forward<Matchers>(matchers)](std::string_view str) {
        return (... and IsMatching(m, str));
    };
}

template <typename T>
class SwitchStr {
    const std::string_view m_str;
    std::optional<T> m_res;

   public:
    constexpr SwitchStr(std::string_view str)
        : m_str(str), m_res(std::nullopt) {}

    template <StrMatcher Matcher>
    constexpr SwitchStr& Case(Matcher&& match, T value) {
        if (not m_res.has_value() and IsMatching(match, m_str)) {
            m_res = std::move(value);
        }
        return *this;
    }

    template <typename U>
    constexpr decltype(auto) Default(U&& value) {
        return m_res.value_or(std::forward<U>(value));
    }

    template <class U = T>
    constexpr operator T() {
        static_assert(sizeof(U) == 0, "Missing a Default() statement");
        return std::move(*m_res);
    }
};

int main(int, char* argv[]) {
    std::size_t pos = std::string_view::npos;
    const int val =
        SwitchStr<int>(0 [argv])
            .Case("titi", 1)
            //.Case(1337, 1)  // Do not compile :+1: !
            .Case(EndsWith("machin"), 2)
            .Case(StartsWith("ttt"), 3)
            .Case(AllOf(StartsWith("patate"), EndsWith("chocolatine")), 4)
            .Case(AnyOf("truc", StartsWith("pouet"), EndsWith("youpi")), 5)
            .Case(Contains('d'), 6)
            .Case(DoNot(ContainsR("TOTO")), 7)
            .Case(ContainsOneOf("0123456789", &pos), 8)
            .Case([](std::string_view str) { return str.empty(); }, 0)
            .Default(-1);

    // TODO: replace return value with Action base functor:
    // Example: 
    // .Case(Something(), Call([](){ ... }))
    // .Case(SomethingElse(), Set(val, 3))
    // ...

    std::printf("val = %d | pos = %zu", val, pos);

    return val;
}
