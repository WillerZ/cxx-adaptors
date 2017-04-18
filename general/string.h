#pragma once

#include <string>
#include <proposed/adaptor>

namespace proposed {

template <typename CharT,
          typename Traits = std::char_traits<CharT>,
          typename Allocator = std::allocator<CharT>>
struct basic_string_adaptor {
  using target_type = std::basic_string<CharT, Traits, Allocator>;
  template <typename X>
  static bool constexpr adapts{
      std::is_same_v<X, std::basic_string_view<CharT, Traits>>};

  void adapt(target_type* pResult,
             std::basic_string_view<CharT, Traits> const& input) {
    ::new (static_cast<void*>(pResult)) target_type(input);
  }
};

using string_adaptor = basic_string_adaptor<char>;
using wstring_adaptor = basic_string_adaptor<wchar_t>;
using u16string_adaptor = basic_string_adaptor<char16_t>;
using u32string_adaptor = basic_string_adaptor<char32_t>;
}  // namespace proposed
