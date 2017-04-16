#pragma once

#include <string>
#include "detail.h"

namespace proposed {
// New Concept: Adapt
//
// The type T satisifies Adapt if it meets the requirements of FunctionObject
// and:
// * defines the type `result_type` (as any non-void type)
// * defines a static constexpr boolean template `adapts<X>` for all types `X`
// Additionally, given:
// * adapter, an object of type T
// * source, a possibly cv-qualified object of type Source
// Either:
// * `T::adapts<Source>` is false
// Or:
// * `T::adapts<Source>` is true; and
// * adapter(source) returns a valid `result_type`

template <typename CharT,
          typename Traits = std::char_traits<CharT>,
          typename Allocator = std::allocator<CharT>>
struct basic_string_adapt {
  using result_type = std::basic_string<CharT, Traits, Allocator>;
  template <typename X>
  static bool constexpr adapts{
      std::is_same_v<X, std::basic_string_view<CharT, Traits>>};

  result_type operator()(std::basic_string_view<CharT, Traits> const& input) {
    return result_type{input};
  }
};

using string_adapt = basic_string_adapt<char>;
using wstring_adapt = basic_string_adapt<wchar_t>;
using u16string_adapt = basic_string_adapt<char16_t>;
using u32string_adapt = basic_string_adapt<char32_t>;

using string_collection_ordered_adapt =
    detail::with_adapt<std::less<>, string_adapt>;
using string_collection_unordered_adapt =
    detail::with_adapt<std::equal_to<>, string_adapt>;
}  // namespace proposed
