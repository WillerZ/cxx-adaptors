#pragma once

#include <set>

namespace proposed {
template <class Key,
          class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
struct multiset : std::multiset<Key, Compare, Allocator> {
 private:
  using this_type = multiset<Key, Compare, Allocator>;
  using base_type = std::multiset<Key, Compare, Allocator>;

 public:
  using base_type::multiset;
#include "base-hack.h"
 private:
  static inline key_type const& value_from_iterator(const_iterator in) {
    return *in;
  }
#include "common-hacky-helpers.h"
 public:
  template <typename AdaptableType>
  typename std::enable_if<is_write_equivalent<AdaptableType>(), iterator>::type
  insert(AdaptableType&& value) {
    auto found = upper_bound(value);
    return insert(found, adapt(std::forward<AdaptableType>(value)));
  }

  template <typename AdaptableType>
  typename std::enable_if<is_write_equivalent<AdaptableType>(), iterator>::type
  insert(const_iterator hint, AdaptableType&& value) {
    return insert(hint, adapt(std::forward<AdaptableType>(value)));
  }

  template <typename AdaptableType>
  typename std::enable_if<is_write_equivalent<AdaptableType const&>()>::type
  insert(std::initializer_list<AdaptableType> ilist) {
    for (const auto& elem : ilist) {
      insert(elem);
    }
  }

  template <typename AdaptableType>
  typename std::enable_if<is_write_equivalent<AdaptableType const&>(),
                          size_type>::type
  erase(const AdaptableType& key) {
    auto range = equal_range(key);
    if (range.first == end()) {
      return 0;
    }
    size_type result{
        static_cast<size_type>(distance(range.first, range.second))};
    if (result) {
      erase(range.first, range.second);
    }
    return result;
  }

  // Can't do emplace or emplace_hint - Ambiguity

  // template <typename AdaptableType>
  // typename std::enable_if<is_write_equivalent<AdaptableType const&>(),
  //                         node_type>::type
  // extract(const AdaptableType& key) {
  //   auto found = findHint(key);
  //   if (found.second) {
  //     return {};
  //   }
  //   return extract(found.first);
  // }
};
}
