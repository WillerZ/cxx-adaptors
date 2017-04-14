#include <proposed/map>
#include <proposed/string>
#include <gtest/gtest.h>
#include <functional>
#include "copy.h"

using MapType =
    proposed::map<std::string, int, proposed::string_adapt<std::less<>>>;

using namespace std::literals;
using namespace test_helpers;

TEST(ProposedMap, ExactKeyType) {
  auto const kHello = "Hello"s;
  auto const kGoodbye = "Goodbye"s;
  auto const kAdios = "Adios"s;
  MapType testMap{};
  testMap[kHello] = 1;
  EXPECT_EQ(1, testMap.at(kHello));
  auto x = testMap.insert_or_assign(kHello, 2);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at(kHello));
  x = testMap.try_emplace(kHello, 3);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at(kHello));
  x = testMap.insert_or_assign(kGoodbye, 4);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(4, x.first->second);
  EXPECT_EQ(4, testMap.at(kGoodbye));
  x = testMap.try_emplace(kAdios, 3);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(3, x.first->second);
  EXPECT_EQ(3, testMap.at(kAdios));
  EXPECT_EQ(1U, testMap.erase(kAdios));
  EXPECT_EQ(0U, testMap.erase(kAdios));
  testMap.clear();
  // And again, with rvalues
  testMap[copy(kHello)] = 1;
  EXPECT_EQ(1, testMap.at(copy(kHello)));
  x = testMap.insert_or_assign(copy(kHello), 2);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at(copy(kHello)));
  x = testMap.try_emplace(copy(kHello), 3);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at(copy(kHello)));
  x = testMap.insert_or_assign(copy(kGoodbye), 4);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(4, x.first->second);
  EXPECT_EQ(4, testMap.at(copy(kGoodbye)));
  x = testMap.try_emplace(copy(kAdios), 3);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(3, x.first->second);
  EXPECT_EQ(3, testMap.at(copy(kAdios)));
  EXPECT_EQ(1U, testMap.erase(copy(kAdios)));
  EXPECT_EQ(0U, testMap.erase(copy(kAdios)));
}

TEST(ProposedMap, ImplicitlyConstructible) {
  MapType testMap{};
  testMap["Hello"] = 1;
  EXPECT_EQ(1, testMap.at("Hello"));
  auto x = testMap.insert_or_assign("Hello", 2);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at("Hello"));
  x = testMap.try_emplace("Hello", 3);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at("Hello"));
  x = testMap.insert_or_assign("Goodbye", 4);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(4, x.first->second);
  EXPECT_EQ(4, testMap.at("Goodbye"));
  x = testMap.try_emplace("Adios", 3);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(3, x.first->second);
  EXPECT_EQ(3, testMap.at("Adios"));
  EXPECT_EQ(1U, testMap.erase("Adios"));
  EXPECT_EQ(0U, testMap.erase("Adios"));
}

TEST(ProposedMap, Adaptable) {
  auto const kHello = "Hello"sv;
  auto const kGoodbye = "Goodbye"sv;
  auto const kAdios = "Adios"sv;
  MapType testMap{};
  testMap[kHello] = 1;
  EXPECT_EQ(1, testMap.at(kHello));
  auto x = testMap.insert_or_assign(kHello, 2);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at(kHello));
  x = testMap.try_emplace(kHello, 3);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at(kHello));
  x = testMap.insert_or_assign(kGoodbye, 4);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(4, x.first->second);
  EXPECT_EQ(4, testMap.at(kGoodbye));
  x = testMap.try_emplace(kAdios, 3);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(3, x.first->second);
  EXPECT_EQ(3, testMap.at(kAdios));
  EXPECT_EQ(1U, testMap.erase(kAdios));
  EXPECT_EQ(0U, testMap.erase(kAdios));
  testMap.clear();
  // And again, with rvalues
  testMap[copy(kHello)] = 1;
  EXPECT_EQ(1, testMap.at(copy(kHello)));
  x = testMap.insert_or_assign(copy(kHello), 2);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at(copy(kHello)));
  x = testMap.try_emplace(copy(kHello), 3);
  EXPECT_EQ(false, x.second);
  EXPECT_EQ(2, x.first->second);
  EXPECT_EQ(2, testMap.at(copy(kHello)));
  x = testMap.insert_or_assign(copy(kGoodbye), 4);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(4, x.first->second);
  EXPECT_EQ(4, testMap.at(copy(kGoodbye)));
  x = testMap.try_emplace(copy(kAdios), 3);
  EXPECT_EQ(true, x.second);
  EXPECT_EQ(3, x.first->second);
  EXPECT_EQ(3, testMap.at(copy(kAdios)));
  EXPECT_EQ(1U, testMap.erase(copy(kAdios)));
  EXPECT_EQ(0U, testMap.erase(copy(kAdios)));
}
