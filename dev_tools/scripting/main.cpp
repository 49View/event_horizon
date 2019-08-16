#define SOL_ALL_SAFETIES_ON 1
#include <lua/sol/sol.hpp> // or #include "sol.hpp", whichever suits your needs

int main(int argc, char* argv[]) {

    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.script("print('Hello world!')");

    return 0;
}