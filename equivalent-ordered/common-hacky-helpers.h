key_adaptor keyAdaptor_;
value_adaptor valueAdaptor_;
static_assert(!adaptor_traits<key_adaptor>::is_adaptor ||
              std::is_same_v<key_type, typename key_adaptor::target_type>);
static_assert(!adaptor_traits<value_adaptor>::is_adaptor ||
              std::is_same_v<value_type, typename value_adaptor::target_type>);

template <typename A, typename AdaptableType>
static bool constexpr is_write_adaptable() {
  return !std::is_same<std::decay_t<Key>, std::decay_t<AdaptableType>>::value &&
         !std::is_same<std::decay_t<iterator>,
                       std::decay_t<AdaptableType>>::value &&
         !std::is_same<std::decay_t<const_iterator>,
                       std::decay_t<AdaptableType>>::value &&
         adaptor_traits<A>::template adapts<AdaptableType>;
}

template <typename A, typename AdaptableType>
typename std::enable_if_t<is_write_adaptable<A, AdaptableType>(), key_type>
adapt(A adaptor, AdaptableType&& adaptee) {
  // Hack to avoid calling the default constructor
  union X {
    X() : x(0) {}
    ~X() noexcept {}
    char x;
    key_type result;
  } x;
  adaptor.adapt(&x.result, std::forward<AdaptableType>(adaptee));
  return std::move(x.result);
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
