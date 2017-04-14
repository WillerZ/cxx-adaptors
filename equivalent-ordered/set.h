#pragma once

#include <set>

namespace proposed {
template <class Key,
          class Compare = std::less<Key>,
          class Allocator = std::allocator<Key> >
struct set : std::set<Key, Compare, Allocator> {
 private:
  using base_type = std::set<Key, Compare, Allocator>;

 public:
  using key_type = typename base_type::key_type;
  using value_type = typename base_type::value_type;
  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;
  using key_compare = typename base_type::key_compare;
  using value_compare = typename base_type::value_compare;
  using allocator_type = typename base_type::allocator_type;
  using reference = typename base_type::reference;
  using const_reference = typename base_type::const_reference;
  using pointer = typename base_type::pointer;
  using const_pointer = typename base_type::const_pointer;
  using iterator = typename base_type::iterator;
  using const_iterator = typename base_type::const_iterator;
  using reverse_iterator = typename base_type::reverse_iterator;
  using const_reverse_iterator = typename base_type::const_reverse_iterator;

  using base_type::set;
  using base_type::operator=;
  using base_type::get_allocator;

  using base_type::begin;
  using base_type::end;
  using base_type::cbegin;
  using base_type::cend;
  using base_type::rbegin;
  using base_type::rend;
  using base_type::crbegin;
  using base_type::crend;

  using base_type::empty;
  using base_type::size;
  using base_type::max_size;

  using base_type::clear;
  using base_type::insert;
  using base_type::emplace;
  using base_type::emplace_hint;
  using base_type::erase;
  using base_type::swap;

  using base_type::count;
  using base_type::find;
  using base_type::equal_range;
  using base_type::lower_bound;
  using base_type::upper_bound;

  using base_type::key_comp;
  using base_type::value_comp;

  /*
    using base_type::extract;
    using base_type::merge;
    using node_type = typename base_type::node_type;
    using insert_return_type = typename base_type::insert_return_type;
    */
};
}
