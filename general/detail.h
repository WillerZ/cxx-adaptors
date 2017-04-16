#pragma once

namespace proposed {
namespace detail {
template <typename FunctionObject, typename Adapt>
struct with_adapt : FunctionObject {
  using FunctionObject::FunctionObject;

  using adapt_type = Adapt;
  adapt_type adapt() { return {}; }
};

template <typename T>
struct has_is_transparent_type {
 private:
  template <typename T1>
  static typename T1::is_transparent* test(int);
  template <typename>
  static void test(...);

 public:
  static constexpr bool value = !std::is_void<decltype(test<T>(0))>::value;
};

template <typename T>
constexpr bool has_is_transparent_type_v{has_is_transparent_type<T>::value};

template <typename T>
struct has_adapt_type {
 private:
  template <typename T1>
  static typename T1::adapt_type* test(int);
  template <typename>
  static void test(...);

 public:
  static constexpr bool value = !std::is_void<decltype(test<T>(0))>::value;
};

template <typename T>
constexpr bool has_adapt_type_v{has_adapt_type<T>::value};

template <typename T, typename V>
struct has_adapt_type_adapting {
 private:
  template <typename V1>
  static constexpr
      typename std::enable_if<(sizeof(V1), has_adapt_type_v<T>), bool>::type
      test(int) {
    return T::adapt_type::template adapts<std::decay_t<V1>>;
  }
  template <typename>
  static constexpr bool test(...) {
    return false;
  }

 public:
  static constexpr bool value = test<V>(0);
};

template <typename T, typename V>
constexpr bool has_adapt_type_adapting_v{has_adapt_type_adapting<T, V>::value};

}  // namespace detail
}  // namespace proposed
