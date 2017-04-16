#pragma once

#include <map>
#include <proposed/detail.h>

namespace proposed {
template <class Key,
          class T,
          class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, T> > >
struct multimap : std::multimap<Key, T, Compare, Allocator> {
 private:
  using this_type = multimap<Key, T, Compare, Allocator>;
  using base_type = std::multimap<Key, T, Compare, Allocator>;

 public:
  using base_type::multimap;
#include "base-hack.h"
 private:
  static inline key_type const& value_from_iterator(const_iterator in) {
    return in->first;
  }
#include "common-hacky-helpers.h"
 public:
  template <typename AdaptableType>
  typename std::enable_if<is_write_adaptable<AdaptableType const&>(),
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

  // Can't do insert, emplace or emplace_hint - Ambiguity

  // template <typename AdaptableType>
  // typename std::enable_if<is_write_adaptable<AdaptableType const&>(),
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
