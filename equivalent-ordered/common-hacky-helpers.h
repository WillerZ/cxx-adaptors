// Base case - not adaptable
static void adapt(...) {
  throw std::logic_error("Should never call this");
}

template <typename AdaptableType>
static auto adapt(AdaptableType&& adaptee)
    -> decltype(std::declval<Compare>().template adapt<key_type>(
        std::forward<AdaptableType>(adaptee))) {
  return Compare().template adapt<key_type>(
      std::forward<AdaptableType>(adaptee));
}

public:
template <typename AdaptableType>
static bool constexpr is_write_equivalent() {
  return !std::is_same<std::decay_t<Key>, std::decay_t<AdaptableType>>::value &&
         !std::is_same<std::decay_t<iterator>,
                       std::decay_t<AdaptableType>>::value &&
         !std::is_same<std::decay_t<const_iterator>,
                       std::decay_t<AdaptableType>>::value &&
         !std::is_same<void,
                       decltype(adapt(std::declval<AdaptableType>()))>::value;
}

private:
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
