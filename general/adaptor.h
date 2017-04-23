#pragma once

namespace proposed {
// New Concept: Adaptor
// The type T satisifies Adaptor if:
// * defines the type `target_type` (as `void`)
// * defines a static constexpr boolean template `adapts<X>` for all types `X`
//
// or:
//
// it meets the requirements of FunctionObject and:
// * defines the type `target_type` (as any non-void type)
// * defines a static constexpr boolean template `adapts<X>` for all types `X`
// Additionally, given:
// * source, a possibly cv-qualified object of type Source
// Either:
// * `T::adapts<Source>` is false
// Or:
// * `T::adapts<Source>` is true; and
// * The instance method:
//       void operator()(target_type *p, Source const& source);
//   is defined and constructs a `target_type` in the uninitialised storage
//   addressed by `p`
// * The instance template method may be optionally defined:
//       void operator()(target_type *p, Source&& source);
//   and is present must construct a `target_type` in the uninitialised storage
//   addressed by `p`

// Default Adaptor for adaptor aware containers so that they have the
// pre-adaptor behaviour
struct no_adaptor {
  using target_type = void;
  template <typename>
  static bool constexpr adapts{false};
};

// Adaptor for any type `Result` which adapts all types for which it has a copy-
// or move-constructor
template <typename Result>
struct constructible_adaptor {
  using target_type = Result;
  template <typename X>
  static bool constexpr adapts{
      std::is_constructible_v<Result, std::decay_t<X> const&> ||
      std::is_constructible_v<Result, std::decay_t<X>&&>};

  template <typename X>
  void operator()(target_type* pResult, X&& input) {
    ::new (static_cast<void*>(pResult)) target_type(std::forward<X>(input));
  }
};

// Adaptor for any type `Result` which adapts all types for which it has a
// non-explicit copy- or move-constructor
template <typename Result>
struct implicit_constructible_adaptor {
  using target_type = Result;
  template <typename X>
  static bool constexpr adapts{
      (std::is_assignable_v<Result, std::decay_t<X> const&> &&
       std::is_constructible_v<Result, std::decay_t<X> const&>) ||
      (std::is_assignable_v<Result, std::decay_t<X>&&> &&
       std::is_constructible_v<Result, std::decay_t<X>&&>)};

  template <typename X>
  void operator()(target_type* pResult, X&& input) {
    ::new (static_cast<void*>(pResult)) target_type(std::forward<X>(input));
  }
};

template <typename Adaptor, typename... Args>
struct union_adaptor {
 private:
  using remainder = union_adaptor<Args...>;
  Adaptor adaptor_;
  remainder remainder_;

 public:
  static_assert(std::is_same_v<Adaptor::target_type, remainder::target_type>,
                "The `target_type` of all adaptor parameters to "
                "`union_adaptor` must be the same");
  union_adaptor() {}
  union_adaptor(Adaptor adaptor, Args... args)
      : adaptor_(adaptor), remainder_(std::move(args)...) {}
  using target_type = typename Adaptor::target_type;
  template <typename X>
  static bool constexpr adapts{Adaptor::template adapts<X> ||
                               remainder::template adapts<X>};

  template <typename X>
  void operator()(target_type* pResult, X&& input) {
    if (adaptor_.adapts<X>) {
      adaptor_(pResult, std::forward<X>(input));
    } else {
      remainder_(pResult, std::forward<X>(input));
    }
  }
};

template <typename Adaptor>
struct union_adaptor<Adaptor> {
 private:
  Adaptor adaptor_;

 public:
  union_adaptor() {}
  union_adaptor(Adaptor adaptor) : adaptor_(adaptor) {}
  using target_type = typename Adaptor::target_type;
  template <typename X>
  static bool constexpr adapts{Adaptor::template adapts<X>};

  template <typename X>
  void operator()(target_type* pResult, X&& input) {
    adaptor_(pResult, std::forward<X>(input));
  }
};

template <typename First,
          typename Second,
          typename Allocator = std::allocator<typename First::target_type>>
struct chain_adaptor {
 private:
  using aTraits = std::allocator_traits<Allocator>;
  Second second_;
  First first_;
  Allocator allocator_;

 public:
  static_assert(Second::template adapts<typename First::target_type>,
                "The second adaptor must be able to adapt the first adaptor's "
                "`target_type`");
  explicit chain_adaptor(First first = {},
                         Second second = {},
                         Allocator allocator = {})
      : first_(first), second_(second), allocator_(allocator) {}
  using target_type = typename Second::target_type;
  template <typename X>
  static bool constexpr adapts{First::template adapts<X>};

  template <typename X>
  void operator()(target_type* pResult, X&& input) {
    auto pIntermediate = aTraits::allocate(allocator_, 1);
    first_(pIntermediate, std::forward<X>(input));
    second_(pResult, std::move(*pIntermediate));
    aTraits::deallocate(allocator_, pIntermediate, 1);
  }
};

namespace detail {
template <typename Adaptor,
          typename Iterator,
          typename Allocator = std::allocator<typename Adaptor::target_type>>
struct adapting_input_iterator {
  using value_type = typename Adaptor::target_type;
  using pointer = value_type const*;
  using reference = value_type const&;
  using difference_type =
      typename std::iterator_traits<Iterator>::difference_type;
  using iterator_category = std::input_iterator_tag;

  adapting_input_iterator() {}

  adapting_input_iterator(adapting_input_iterator&& other)
      : adaptor_(std::move(other.adaptor_)),
        underlying_(std::move(other.underlying_)),
        value_(other.value_) {
    other.value_ = nullptr;
  }
  adapting_input_iterator(adapting_input_iterator const& other)
      : adaptor_(other.adaptor_), underlying_(other.underlying_) {
    if (other.value_) {
      ensureValue();
    }
  }

  explicit adapting_input_iterator(Adaptor adaptor, Allocator allocator = {})
      : adaptor_(adaptor), allocator_(allocator) {}
  explicit adapting_input_iterator(Iterator underlying,
                                   Allocator allocator = {})
      : underlying_(underlying), allocator_(allocator) {}
  adapting_input_iterator(Adaptor adaptor,
                          Iterator underlying,
                          Allocator allocator = {})
      : adaptor_(adaptor), underlying_(underlying), allocator_(allocator) {}
  ~adapting_input_iterator() { ensureNoValue(); }

  adapting_input_iterator& operator=(adapting_input_iterator&& other) {
    ensureNoValue();
    ::new (static_cast<void*>(this)) adapting_input_iterator(std::move(other));
    return *this;
  }
  adapting_input_iterator& operator=(adapting_input_iterator const& other) {
    ensureNoValue();
    ::new (static_cast<void*>(this)) adapting_input_iterator(other);
    return *this;
  }

  bool operator==(adapting_input_iterator const& other) const {
    return adaptor_ == other.adaptor_ && underlying_ == other.underlying_;
  }
  bool operator!=(adapting_input_iterator const& other) const {
    return !operator==(other);
  }
  bool operator==(Iterator const& other) const { return underlying_ == other; }
  bool operator!=(Iterator const& other) const { return !operator==(other); }

  adapting_input_iterator& operator++() {
    ensureNoValue();
    ++underlying_;
    return *this;
  }

  adapting_input_iterator operator++(int) {
    adapting_input_iterator result{};
    result.adaptor_ = adaptor_;
    result.underlying_ = underlying_;
    result.value_ = value_;
    value_ = nullptr;
    ++underlying_;
    return result;
  }

  pointer operator->() {
    ensureValue();
    return value_;
  }
  reference operator*() {
    ensureValue();
    return *value_;
  }

 private:
  using aTraits = std::allocator_traits<Allocator>;
  void ensureValue() {
    if (value_) {
      return;
    }
    value_ = aTraits::allocate(allocator_, 1);
    adaptor_(value_, *underlying_);
  }
  void ensureNoValue() {
    if (!value_) {
      return;
    }
    value_->~value_type();
    aTraits::deallocate(allocator_, value_, 1);
    value_ = nullptr;
  }
  Adaptor adaptor_;
  Iterator underlying_;
  Allocator allocator_;
  value_type* value_;
};

template <typename Adaptor,
          typename Iterator,
          typename Allocator = std::allocator<typename Adaptor::target_type>>
struct adapting_output_iterator {
  using iterator_category = std::output_iterator_tag;

  adapting_output_iterator() {}

  adapting_output_iterator(adapting_output_iterator&& other)
      : adaptor_(std::move(other.adaptor_)),
        underlying_(std::move(other.underlying_)) {}
  adapting_output_iterator(adapting_output_iterator const& other)
      : adaptor_(other.adaptor_), underlying_(other.underlying_) {}

  explicit adapting_output_iterator(Adaptor adaptor, Allocator allocator = {})
      : adaptor_(adaptor), allocator_(allocator) {}
  explicit adapting_output_iterator(Iterator underlying,
                                    Allocator allocator = {})
      : underlying_(underlying), allocator_(allocator) {}
  adapting_output_iterator(Adaptor adaptor,
                           Iterator underlying,
                           Allocator allocator = {})
      : adaptor_(adaptor), underlying_(underlying), allocator_(allocator) {}

  adapting_output_iterator& operator=(adapting_output_iterator&& other) {
    ::new (static_cast<void*>(this)) adapting_output_iterator(std::move(other));
    return *this;
  }
  adapting_output_iterator& operator=(adapting_output_iterator const& other) {
    ::new (static_cast<void*>(this)) adapting_output_iterator(other);
    return *this;
  }

  adapting_output_iterator& operator++() {
    ++underlying_;
    return *this;
  }

  adapting_output_iterator operator++(int) {
    adapting_output_iterator result{*this};
    operator++();
    return result;
  }

  class assignment_proxy {
    using aTraits = std::allocator_traits<Allocator>;
    Adaptor adaptor_;
    Iterator underlying_;
    Allocator allocator_;

   public:
    template <typename T>
    typename std::enable_if<Adaptor::template adapts<T>>::type operator=(
        T&& thing) {
      using result_type = typename Adaptor::target_type;
      result_type* result = aTraits::allocate(allocator_, 1);
      adaptor_(result, std::forward<T>(thing));
      *underlying_ = std::move(result);
      result->~result_type();
      aTraits::deallocate(allocator_, result, 1);
    }
    assignment_proxy(Adaptor adaptor, Iterator underlying, Allocator allocator)
        : adaptor_(adaptor), underlying_(underlying), allocator_(allocator) {}
  };

  assignment_proxy operator*() { return {adaptor_, underlying_, allocator_}; }

 private:
  Adaptor adaptor_;
  Iterator underlying_;
  Allocator allocator_;
};
}

/**
 * Return value is an unspecified input iterator. You can usefully use
 * std::make_move_iterator on the parameter iterator and on the returned
 * iterator.
 */
template <typename Adaptor,
          typename Iterator,
          typename Allocator = std::allocator<typename Adaptor::target_type>>
typename std::enable_if<
    !std::is_same_v<std::decay_t<typename Adaptor::target_type>,
                    std::decay_t<typename Iterator::value_type>>,
    detail::adapting_input_iterator<Adaptor, Iterator, Allocator>>::type
adapt_input_iterator(Iterator&& iterator, Allocator&& allocator = {}) {
  return detail::adapting_input_iterator<Adaptor, Iterator, Allocator>{
      std::forward<Iterator>(iterator), std::forward<Allocator>(allocator)};
}

template <typename Adaptor, typename Iterator, typename = void>
typename std::enable_if<
    std::is_same_v<std::decay_t<typename Adaptor::target_type>,
                   std::decay_t<typename Iterator::value_type>>,
    Iterator>::type
adapt_input_iterator(Iterator&& iterator, ...) {
  return std::forward<Iterator>(iterator);
}

/**
 * Return value is an unspecified output iterator.
 */
template <typename Adaptor,
          typename Iterator,
          typename Allocator = std::allocator<typename Adaptor::target_type>>
typename std::enable_if<
    !std::is_same_v<std::decay_t<typename Adaptor::target_type>,
                    std::decay_t<typename Iterator::value_type>>,
    detail::adapting_output_iterator<Adaptor, Iterator, Allocator>>::type
adapt_output_iterator(Iterator&& iterator, Allocator&& allocator = {}) {
  return detail::adapting_output_iterator<Adaptor, Iterator, Allocator>{
      std::forward<Iterator>(iterator), std::forward<Allocator>(allocator)};
}

template <typename Adaptor, typename Iterator, typename = void>
typename std::enable_if<
    std::is_same_v<std::decay_t<typename Adaptor::target_type>,
                   std::decay_t<typename Iterator::value_type>>,
    Iterator>::type
adapt_output_iterator(Iterator&& iterator, ...) {
  return std::forward<Iterator>(iterator);
}

// Utility class to check if class T defines an Adaptor type named adaptor_type
// which can adapt type V.
template <typename Adaptor>
struct adaptor_traits {
 private:
  template <typename A>
  static constexpr typename A::target_type testResultType(int);
  template <typename>
  static constexpr void testResultType(...);

 public:
  static constexpr bool is_adaptor =
      !std::is_void_v<decltype(testResultType<Adaptor>(0))>;

 private:
  template <typename T>
  static constexpr typename std::enable_if<(sizeof(T), is_adaptor), bool>::type
  testAdaptType(int) {
    return Adaptor::template adapts<std::decay_t<T>>;
  }
  template <typename>
  static constexpr bool testAdaptType(...) {
    return false;
  }

 public:
  template <typename T>
  static constexpr bool adapts = testAdaptType<T>(0);
};

}  // namespace proposed
