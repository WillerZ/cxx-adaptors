#pragma once

#include <set>
#include <proposed/adaptor>

namespace proposed {
template <class Key,
          class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>,
          class Adaptor = no_adaptor>
struct set : std::set<Key, Compare, Allocator> {
 private:
  using this_type = set<Key, Compare, Allocator>;
  using base_type = std::set<Key, Compare, Allocator>;
  using key_adaptor = Adaptor;
  using value_adaptor = Adaptor;

 public:
  using base_type::set;
#include "base-hack.h"
 private:
  static inline key_type const& value_from_iterator(const_iterator in) {
    return *in;
  }
#include "common-hacky-helpers.h"
 public:
  template <typename AdaptableType>
  typename std::enable_if<is_write_adaptable<key_adaptor, AdaptableType>(),
                          std::pair<iterator, bool>>::type
  insert(AdaptableType&& value) {
    auto found = findHint(value);
    if (!found.second) {
      return found;
    }
    return {insert(found.first,
                   adapt<key_adaptor>(keyAdaptor_,
                                      std::forward<AdaptableType>(value))),
            true};
  }

  template <typename AdaptableType>
  typename std::enable_if<is_write_adaptable<key_adaptor, AdaptableType>(),
                          iterator>::type
  insert(const_iterator hint, AdaptableType&& value) {
    auto found = findHint(hint, value);
    if (!found.second) {
      return get_iterator(found.first);
    }
    return insert(
        found.first,
        adapt<key_adaptor>(keyAdaptor_, std::forward<AdaptableType>(value)));
  }

  template <typename AdaptableType>
  typename std::enable_if<
      is_write_adaptable<key_adaptor, AdaptableType const&>()>::type
  insert(std::initializer_list<AdaptableType> ilist) {
    for (auto const& elem : ilist) {
      insert(elem);
    }
  }

  template <typename AdaptableType>
  typename std::enable_if<
      is_write_adaptable<key_adaptor, AdaptableType const&>(),
      size_type>::type
  erase(const AdaptableType& key) {
    auto found = findHint(key);
    if (found.second) {
      return 0;
    }
    erase(found.first);
    return 1;
  }

  // Can't do emplace or emplace_hint - Ambiguity

  // template <typename AdaptableType>
  // typename std::enable_if<is_write_adaptable<key_adaptor, AdaptableType
  // const&>(),
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
