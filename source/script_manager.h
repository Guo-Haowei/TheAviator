#pragma once
#include <string>

inline constexpr const char LUA_SCRIPT_SETUP[] = "setup.lua";

struct lua_State;

class ScriptManager
{
public:
    bool init();
    void deinit();
    bool doFile(const char* file);

private:
    void reportError();
    void openLibs();

    static std::string buildLuaSrcFolder();

    lua_State* m_state = nullptr;
};
