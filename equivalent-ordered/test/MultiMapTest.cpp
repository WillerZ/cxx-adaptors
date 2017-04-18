#include <proposed/multimap>
#include <proposed/string>
#include <gtest/gtest.h>
#include <functional>

using MultiMapType =
    proposed::multimap<std::string,
                       int,
                       std::less<>,
                       std::allocator<std::pair<const std::string, int>>,
                       proposed::string_adaptor>;

using namespace std::literals;

TEST(ProposedMultiMap, ExactKeyType) {
  MultiMapType testMultiMap{};
  testMultiMap.emplace("Hello"s, 1);
  testMultiMap.emplace("Hello"s, 2);
  testMultiMap.emplace("Hello"s, 3);
  testMultiMap.emplace("Hello"s, 4);
  testMultiMap.emplace("Aye"s, 1);
  testMultiMap.emplace("Aye"s, 2);
  testMultiMap.emplace("Zoom"s, 3);
  testMultiMap.emplace("Zoom"s, 4);
  EXPECT_EQ(8u, testMultiMap.size());
  EXPECT_EQ(4u, testMultiMap.count("Hello"s));
  EXPECT_EQ(4u, testMultiMap.erase("Hello"s));
  EXPECT_EQ(0u, testMultiMap.count("Hello"s));
  EXPECT_EQ(4u, testMultiMap.size());
}

TEST(ProposedMultiMap, ImplicitlyConstructible) {
  MultiMapType testMultiMap{};
  testMultiMap.emplace("Hello", 1);
  testMultiMap.emplace("Hello", 2);
  testMultiMap.emplace("Hello", 3);
  testMultiMap.emplace("Hello", 4);
  testMultiMap.emplace("Aye", 1);
  testMultiMap.emplace("Aye", 2);
  testMultiMap.emplace("Zoom", 3);
  testMultiMap.emplace("Zoom", 4);
  EXPECT_EQ(8u, testMultiMap.size());
  EXPECT_EQ(4u, testMultiMap.count("Hello"));
  EXPECT_EQ(4u, testMultiMap.erase("Hello"));
  EXPECT_EQ(0u, testMultiMap.count("Hello"));
  EXPECT_EQ(4u, testMultiMap.size());
}

TEST(ProposedMultiMap, Adaptable) {
  MultiMapType testMultiMap{};
  testMultiMap.emplace("Hello"s, 1);
  testMultiMap.emplace("Hello"s, 2);
  testMultiMap.emplace("Hello"s, 3);
  testMultiMap.emplace("Hello"s, 4);
  testMultiMap.emplace("Aye"s, 1);
  testMultiMap.emplace("Aye"s, 2);
  testMultiMap.emplace("Zoom"s, 3);
  testMultiMap.emplace("Zoom"s, 4);
  EXPECT_EQ(8u, testMultiMap.size());
  EXPECT_EQ(4u, testMultiMap.count("Hello"sv));
  EXPECT_EQ(4u, testMultiMap.erase("Hello"sv));
  EXPECT_EQ(0u, testMultiMap.count("Hello"sv));
  EXPECT_EQ(4u, testMultiMap.size());
}
