// main.cc
#include <gameEngine/Game.h>

#include "script_manager.h"

int main()
{
    Game::init();

    ScriptManager manager;
    if(!manager.init())
    {
        return -1;
    }

    if(!manager.doFile(LUA_SCRIPT_SETUP))
    {
        return -1;
    }

    while(Game::theOne().shouldRun())
    {
        Game::theOne().run();
    }

    manager.deinit();

    return 0;
}
