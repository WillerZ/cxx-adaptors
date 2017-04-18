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
