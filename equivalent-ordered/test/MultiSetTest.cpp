#include <proposed/multiset>
#include <proposed/string>
#include <gtest/gtest.h>
#include <functional>
#include "copy.h"

using MultiSetType =
    proposed::multiset<std::string, proposed::string_adapt<std::less<>>>;

using namespace std::literals;
using namespace test_helpers;

TEST(ProposedMultiSet, ExactValueType) {
  auto const kHello = "Hello"s;
  auto const kMultiSet = "MultiSet"s;
  auto const kWorld = "World"s;
  std::initializer_list<std::string> kList = {kHello, kMultiSet, kWorld};
  MultiSetType testMultiSet{};
  testMultiSet.insert(kHello);
  EXPECT_EQ(1U, testMultiSet.count(kHello));
  testMultiSet.insert(kList);
  EXPECT_EQ(2U, testMultiSet.count(kHello));
  EXPECT_EQ(1U, testMultiSet.count(kMultiSet));
  EXPECT_EQ(1U, testMultiSet.count(kWorld));
  EXPECT_EQ(2U, testMultiSet.erase(kHello));
  EXPECT_EQ(0U, testMultiSet.erase(kHello));
  testMultiSet.clear();
  // And again, with rvalues
  testMultiSet.insert(copy(kHello));
  EXPECT_EQ(1U, testMultiSet.count(copy(kHello)));
  testMultiSet.insert(copy(kList));
  EXPECT_EQ(2U, testMultiSet.count(copy(kHello)));
  EXPECT_EQ(1U, testMultiSet.count(copy(kMultiSet)));
  EXPECT_EQ(1U, testMultiSet.count(copy(kWorld)));
  EXPECT_EQ(2U, testMultiSet.erase(copy(kHello)));
  EXPECT_EQ(0U, testMultiSet.erase(copy(kHello)));
}

TEST(ProposedMultiSet, ImplicitlyConstructible) {
  MultiSetType testMultiSet{};
  testMultiSet.insert("Hello");
  EXPECT_EQ(1U, testMultiSet.count("Hello"));
  testMultiSet.insert({"Hello", "MultiSet", "World"});
  EXPECT_EQ(2U, testMultiSet.count("Hello"));
  EXPECT_EQ(1U, testMultiSet.count("MultiSet"));
  EXPECT_EQ(1U, testMultiSet.count("World"));
  EXPECT_EQ(2U, testMultiSet.erase("Hello"));
  EXPECT_EQ(0U, testMultiSet.erase("Hello"));
}

TEST(ProposedMultiSet, Adaptable) {
  auto const kHello = "Hello"sv;
  auto const kMultiSet = "MultiSet"sv;
  auto const kWorld = "World"sv;
  std::initializer_list<std::string_view> kList = {kHello, kMultiSet, kWorld};
  MultiSetType testMultiSet{};
  testMultiSet.insert(kHello);
  EXPECT_EQ(1U, testMultiSet.count(kHello));
  testMultiSet.insert(kList);
  EXPECT_EQ(2U, testMultiSet.count(kHello));
  EXPECT_EQ(1U, testMultiSet.count(kMultiSet));
  EXPECT_EQ(1U, testMultiSet.count(kWorld));
  EXPECT_EQ(2U, testMultiSet.erase(kHello));
  EXPECT_EQ(0U, testMultiSet.erase(kHello));
  testMultiSet.clear();
  // And again, with rvalues
  testMultiSet.insert(copy(kHello));
  EXPECT_EQ(1U, testMultiSet.count(copy(kHello)));
  testMultiSet.insert(copy(kList));
  EXPECT_EQ(2U, testMultiSet.count(copy(kHello)));
  EXPECT_EQ(1U, testMultiSet.count(copy(kMultiSet)));
  EXPECT_EQ(1U, testMultiSet.count(copy(kWorld)));
  EXPECT_EQ(2U, testMultiSet.erase(copy(kHello)));
  EXPECT_EQ(0U, testMultiSet.erase(copy(kHello)));
}
