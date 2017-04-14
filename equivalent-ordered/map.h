#pragma once

#include <map>

namespace proposed {
template <class Key,
          class T,
          class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, T>>>
struct map : std::map<Key, T, Compare, Allocator> {
 private:
  using this_type = map<Key, T, Compare, Allocator>;
  using base_type = std::map<Key, T, Compare, Allocator>;

 public:
  using base_type::map;
  using base_type::operator[];
  using base_type::at;
  using base_type::insert_or_assign;
  using base_type::try_emplace;
#include "base-hack.h"
 private:
  static inline key_type const& value_from_iterator(const_iterator in) {
    return in->first;
  }
#include "common-hacky-helpers.h"
 public:
  template <typename AdaptableType, typename M>
  typename std::enable_if<is_write_equivalent<AdaptableType>(),
                          std::pair<iterator, bool>>::type
  insert_or_assign(AdaptableType&& key, M&& obj) {
    auto found = findHint(key);
    if (!found.second) {
      found.first->second = std::forward<M>(obj);
      return found;
    }
    return {this->base_type::insert_or_assign(
                found.first, adapt(std::forward<AdaptableType>(key)),
                std::forward<M>(obj)),
            true};
  }

  template <typename AdaptableType, typename M>
  typename std::enable_if<is_write_equivalent<AdaptableType>(), iterator>::type
  insert_or_assign(const_iterator hint, AdaptableType&& key, M&& obj) {
    auto found = findHint(hint, key);
    if (!found.second) {
      auto result = get_iterator(found.first);
      result->second = std::forward<M>(obj);
      return result;
    }
    return this->base_type::insert_or_assign(
        found.first, adapt(std::forward<AdaptableType>(key)),
        std::forward<M>(obj));
  }

  template <typename AdaptableType, typename... Args>
  typename std::enable_if<is_write_equivalent<AdaptableType>(),
                          std::pair<iterator, bool>>::type
  try_emplace(AdaptableType&& key, Args&&... args) {
    auto found = findHint(key);
    if (!found.second) {
      return found;
    }
    return {this->base_type::try_emplace(
                found.first, adapt(std::forward<AdaptableType>(key)),
                std::forward<Args>(args)...),
            true};
  }

  template <typename AdaptableType, typename... Args>
  typename std::enable_if<is_write_equivalent<AdaptableType>(), iterator>::type
  try_emplace(const_iterator hint, AdaptableType&& key, Args&&... args) {
    auto found = findHint(hint, key);
    if (!found.second) {
      return get_iterator(found.first);
    }
    return this->base_type::try_emplace(found.first,
                                        adapt(std::forward<AdaptableType>(key)),
                                        std::forward<Args>(args)...);
  }

  template <typename AdaptableType>
  typename std::enable_if<is_write_equivalent<AdaptableType const&>(),
                          size_type>::type
  erase(AdaptableType const& key) {
    auto found = findHint(key);
    if (found.second) {
      return 0;
    }
    this->base_type::erase(found.first);
    return 1;
  }

  template <typename AdaptableType>
  typename std::enable_if<is_write_equivalent<AdaptableType const&>(), T&>::type
  at(AdaptableType const& key) {
    auto found = findHint(key);
    if (found.second) {
      throw std::out_of_range{"No such key in map"};
    }
    return found.first->second;
  }

  template <typename AdaptableType>
  typename std::enable_if<is_write_equivalent<AdaptableType const&>(),
                          T const&>::type
  at(AdaptableType const& key) const {
    auto found = findHint(key);
    if (found.second) {
      throw std::out_of_range{"No such key in map"};
    }
    return found.first->second;
  }

  template <typename AdaptableType>
  typename std::enable_if<is_write_equivalent<AdaptableType>(), T&>::type
  operator[](AdaptableType&& key) {
    return try_emplace(std::forward<AdaptableType>(key)).first->second;
  }

  // Can't do emplace or emplace_hint - Ambiguity

  // template <typename AdaptableType>
  // typename std::enable_if<is_write_equivalent<AdaptableType,0>(),
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
