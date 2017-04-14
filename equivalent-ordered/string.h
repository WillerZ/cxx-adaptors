#pragma once

#include <string>

// New Concept: AdaptingCompare
//
// The type T satisifies AdaptingCompare if:
// * the type T satisfies Compare, and
// Given:
// * adapter, an object of type AdaptingCompare
// * source, an object of type Source
// The following expression must be valid:
// * adapter.adapt<Target, Source>(source)
// and have one of the following return types:
// * Target
// * void
// If this expression has the return type Target then for all a:
// * if adapter(a, source) is defined:
//   * adapter(a, adapter.adapt<Target, Source>(source)) is defined, and
//     has an equivalent result when contextually converted to bool
// * if adapter(source, a) is defined:
//   * adapter(adapter.adapt<Target, Source>(source), a) is defined, and
//     has an equivalent result when contextually converted to bool

namespace proposed {
template <typename Compare>
struct string_adapt : Compare {
  using Compare::Compare;
  using Compare::operator();

 private:
  template <typename X>
  void adapt_helper(X&&, long) {}

  template <typename CharT, typename Traits, typename Allocator>
  auto adapt_helper(std::basic_string_view<CharT, Traits> const& source, int)
      -> decltype(std::basic_string<CharT, Traits, Allocator>{source}) {
    return std::basic_string<CharT, Traits, Allocator>{source};
  }

 public:
  template <typename Target, typename Source>
  auto adapt(Source const& source)
      -> decltype(adapt_helper<typename Target::value_type,
                               typename Target::traits_type,
                               typename Target::allocator_type>(source, 0)) {
    return adapt_helper<typename Target::value_type,
                        typename Target::traits_type,
                        typename Target::allocator_type>(source, 0);
  }
};
}  // namespace proposed
