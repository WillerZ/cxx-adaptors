template <typename AdaptableType>
static bool constexpr is_write_adaptable() {
  return !std::is_same<std::decay_t<Key>, std::decay_t<AdaptableType>>::value &&
         !std::is_same<std::decay_t<iterator>,
                       std::decay_t<AdaptableType>>::value &&
         !std::is_same<std::decay_t<const_iterator>,
                       std::decay_t<AdaptableType>>::value &&
         detail::has_adapt_type_adapting_v<key_compare, AdaptableType>;
}

template <typename AdaptableType>
typename std::enable_if_t<is_write_adaptable<AdaptableType>(), key_type> adapt(
    AdaptableType&& adaptee) {
  return key_comp().adapt()(std::forward<AdaptableType>(adaptee));
}

// The bool is false if the element is already present
template <typename AdaptableType>
std::pair<iterator, bool> findHint(AdaptableType const& value) {
  Compare compare{};
  auto hint = lower_bound(value);
  return {hint, hint == end() || compare(value, value_from_iterator(hint))};
}

// The bool is false if the element is already present
template <typename AdaptableType>
std::pair<const_iterator, bool> findHint(AdaptableType const& value) const {
  Compare compare{};
  auto hint = lower_bound(value);
  return {hint, hint == end() || compare(value, value_from_iterator(hint))};
}

// The bool is false if the element is already present
template <typename AdaptableType>
std::pair<const_iterator, bool> findHint(const_iterator hint,
                                         AdaptableType const& value) const {
  Compare compare{};
  if (hint != end() && compare(value_from_iterator(hint), value)) {
    // Bad initial hint
    hint = lower_bound(value);
  } else if (hint != begin()) {
    const_iterator prev = hint;
    --prev;
    if (!compare(value_from_iterator(hint), value)) {
      // Bad initial hint
      hint = lower_bound(value);
    }
  }
  return {hint, hint == end() || compare(value, value_from_iterator(hint))};
}

iterator get_iterator(const_iterator in) {
  if (in == cend()) {
    return end();
  }
  return insert(in, *in);
}
