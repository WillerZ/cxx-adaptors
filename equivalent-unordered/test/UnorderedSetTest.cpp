#include <proposed/unordered_set>
#include <gtest/gtest.h>
#include <functional>
#include <string>
#include <test-utils/copy.h>
#include <iostream>

using SetType = proposed::unordered_set<std::string>;

using namespace std::literals;
using namespace test_utils;

TEST(ProposedUnorderedSet, ExactValueType) {
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

TEST(ProposedUnorderedSet, ImplicitlyConstructible) {
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