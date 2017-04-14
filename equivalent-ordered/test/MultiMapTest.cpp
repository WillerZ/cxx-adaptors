#include <proposed/multimap>
#include <proposed/string>
#include <functional>

static proposed::multimap<std::string, int, proposed::string_adapt<std::less<>>>
    testMap{};
