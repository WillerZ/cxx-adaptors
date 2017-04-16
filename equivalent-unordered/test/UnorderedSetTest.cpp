#include <proposed/unordered_set>
#include <proposed/string>
#include <gtest/gtest.h>
#include <functional>
#include <string>
#include <test-utils/copy.h>
#include <iostream>

struct myhash : std::hash<std::string_view> {
  using is_transparent = void;
};

using SetType = proposed::
    unordered_set<std::string, myhash, proposed::string_adapt<std::equal_to<>>>;

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
  char const* kHello = "Hello";
  char const* kSet = "Set";
  char const* kWorld = "World";
  SetType testSet{};
  std::initializer_list<std::string> kList = {
      std::string{kHello}, std::string{kSet}, std::string{kWorld}};
  testSet.insert(kHello);
  EXPECT_EQ(1U, testSet.count(kHello));
  testSet.insert(kList);
  EXPECT_EQ(1U, testSet.count(kHello));
  EXPECT_EQ(1U, testSet.count(kSet));
  EXPECT_EQ(1U, testSet.count(kWorld));
  EXPECT_EQ(1U, testSet.erase(kHello));
  EXPECT_EQ(0U, testSet.erase(kHello));
}

TEST(ProposedUnorderedSet, TransparentLookup) {
  auto const kHello = "Hello"sv;
  auto const kSet = "Set"sv;
  auto const kWorld = "World"sv;
  std::initializer_list<std::string> kList = {
      std::string{kHello}, std::string{kSet}, std::string{kWorld}};
  SetType testSet{};
  testSet.insert(std::string{kHello});
  EXPECT_EQ(1U, testSet.count(kHello));
  testSet.insert(kList);
  EXPECT_EQ(1U, testSet.count(kHello));
  EXPECT_EQ(1U, testSet.count(kSet));
  EXPECT_EQ(1U, testSet.count(kWorld));
  EXPECT_EQ(1U, testSet.erase(kHello));
  EXPECT_EQ(0U, testSet.erase(kHello));
  testSet.clear();
  // And again, with rvalues
  testSet.insert(std::string{kHello});
  EXPECT_EQ(1U, testSet.count(copy(kHello)));
  testSet.insert(copy(kList));
  EXPECT_EQ(1U, testSet.count(copy(kHello)));
  EXPECT_EQ(1U, testSet.count(copy(kSet)));
  EXPECT_EQ(1U, testSet.count(copy(kWorld)));
  EXPECT_EQ(1U, testSet.erase(copy(kHello)));
  EXPECT_EQ(0U, testSet.erase(copy(kHello)));
}

TEST(ProposedUnorderedSet, Adaptable) {
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
  testSet.insert(kHello);
  EXPECT_EQ(1U, testSet.count(copy(kHello)));
  testSet.insert(copy(kList));
  EXPECT_EQ(1U, testSet.count(copy(kHello)));
  EXPECT_EQ(1U, testSet.count(copy(kSet)));
  EXPECT_EQ(1U, testSet.count(copy(kWorld)));
  EXPECT_EQ(1U, testSet.erase(copy(kHello)));
  EXPECT_EQ(0U, testSet.erase(copy(kHello)));
}
