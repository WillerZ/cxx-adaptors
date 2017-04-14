#include <proposed/set>
#include <proposed/string>
#include <gtest/gtest.h>
#include <functional>
#include "copy.h"

using SetType = proposed::set<std::string, proposed::string_adapt<std::less<>>>;

using namespace std::literals;
using namespace test_helpers;

TEST(ProposedSet, ExactValueType) {
  auto const kHello = "Hello"s;
  auto const kSet = "Set"s;
  auto const kWorld = "World"s;
  std::initializer_list<std::string> kList = {kHello, kSet, kWorld};
  SetType testSet{};
  testSet.insert(kHello);
  EXPECT_EQ(1U, testSet.count(kHello));
  testSet.insert(kList);
  EXPECT_EQ(1U, testSet.count(kHello));
  EXPECT_EQ(1U, testSet.count(kSet));
  EXPECT_EQ(1U, testSet.count(kWorld));
  EXPECT_EQ(1U, testSet.erase(kHello));
  EXPECT_EQ(0U, testSet.erase(kHello));
  testSet.clear();
  // And again, with rvalues
  testSet.insert(copy(kHello));
  EXPECT_EQ(1U, testSet.count(copy(kHello)));
  testSet.insert(copy(kList));
  EXPECT_EQ(1U, testSet.count(copy(kHello)));
  EXPECT_EQ(1U, testSet.count(copy(kSet)));
  EXPECT_EQ(1U, testSet.count(copy(kWorld)));
  EXPECT_EQ(1U, testSet.erase(copy(kHello)));
  EXPECT_EQ(0U, testSet.erase(copy(kHello)));
}

TEST(ProposedSet, ImplicitlyConstructible) {
  SetType testSet{};
  testSet.insert("Hello");
  EXPECT_EQ(1U, testSet.count("Hello"));
  testSet.insert({"Hello", "Set", "World"});
  EXPECT_EQ(1U, testSet.count("Hello"));
  EXPECT_EQ(1U, testSet.count("Set"));
  EXPECT_EQ(1U, testSet.count("World"));
  EXPECT_EQ(1U, testSet.erase("Hello"));
  EXPECT_EQ(0U, testSet.erase("Hello"));
}

TEST(ProposedSet, Adaptable) {
  auto const kHello = "Hello"sv;
  auto const kSet = "Set"sv;
  auto const kWorld = "World"sv;
  std::initializer_list<std::string_view> kList = {kHello, kSet, kWorld};
  SetType testSet{};
  testSet.insert(kHello);
  EXPECT_EQ(1U, testSet.count(kHello));
  testSet.insert(kList);
  EXPECT_EQ(1U, testSet.count(kHello));
  EXPECT_EQ(1U, testSet.count(kSet));
  EXPECT_EQ(1U, testSet.count(kWorld));
  EXPECT_EQ(1U, testSet.erase(kHello));
  EXPECT_EQ(0U, testSet.erase(kHello));
  testSet.clear();
  // And again, with rvalues
  testSet.insert(copy(kHello));
  EXPECT_EQ(1U, testSet.count(copy(kHello)));
  testSet.insert(copy(kList));
  EXPECT_EQ(1U, testSet.count(copy(kHello)));
  EXPECT_EQ(1U, testSet.count(copy(kSet)));
  EXPECT_EQ(1U, testSet.count(copy(kWorld)));
  EXPECT_EQ(1U, testSet.erase(copy(kHello)));
  EXPECT_EQ(0U, testSet.erase(copy(kHello)));
}
