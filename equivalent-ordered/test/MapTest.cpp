#include <proposed/map>
#include <proposed/string>
#include <functional>

static proposed::map<std::string, int, proposed::string_adapt<std::less<>>>
    testMap{};
