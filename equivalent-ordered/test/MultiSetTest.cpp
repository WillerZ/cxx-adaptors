#include <proposed/multiset>
#include <proposed/string>
#include <functional>

static proposed::multiset<std::string, proposed::string_adapt<std::less<>>>
    testSet{};
