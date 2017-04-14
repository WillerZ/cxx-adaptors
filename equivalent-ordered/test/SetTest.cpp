#include <proposed/set>
#include <proposed/string>
#include <functional>

static proposed::set<std::string, proposed::string_adapt<std::less<>>>
    testSet{};
