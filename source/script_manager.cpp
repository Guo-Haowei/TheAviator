#include "script_manager.h"

#include "entities/Entity.h"
#include "models/Geometry.h"
#include "models/Loader.h"
#include "models/RawModel.h"

#include <algorithm>
#include <cassert>
#include <vector>

extern "C"
{
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

// temp
constexpr size_t NUM_CUBE_VERTICES = 8 * 3;
static RawModel* createCube(const std::vector<float>& points)
{
    assert(points.size() == NUM_CUBE_VERTICES);

    const glm::vec3 vert0 = glm::vec3(points[0], points[1], points[2]);
    const glm::vec3 vert1 = glm::vec3(points[3], points[4], points[5]);
    const glm::vec3 vert2 = glm::vec3(points[6], points[7], points[8]);
    const glm::vec3 vert3 = glm::vec3(points[9], points[10], points[11]);
    const glm::vec3 vert4 = glm::vec3(points[12], points[13], points[14]);
    const glm::vec3 vert5 = glm::vec3(points[15], points[16], points[17]);
    const glm::vec3 vert6 = glm::vec3(points[18], points[19], points[20]);
    const glm::vec3 vert7 = glm::vec3(points[21], points[22], points[23]);

    glm::vec3 vertices[36];
    // face left
    vertices[0] = vert0;
    vertices[1] = vert1;
    vertices[2] = vert3;
    vertices[3] = vert0;
    vertices[4] = vert3;
    vertices[5] = vert2;
    // face right
    vertices[6] = vert4;
    vertices[7] = vert5;
    vertices[8] = vert6;
    vertices[9] = vert5;
    vertices[10] = vert7;
    vertices[11] = vert6;
    // face front
    vertices[12] = vert0;
    vertices[13] = vert2;
    vertices[14] = vert7;
    vertices[15] = vert0;
    vertices[16] = vert7;
    vertices[17] = vert5;
    // face back
    vertices[18] = vert1;
    vertices[19] = vert6;
    vertices[20] = vert3;
    vertices[21] = vert1;
    vertices[22] = vert4;
    vertices[23] = vert6;
    // face up
    vertices[24] = vert4;
    vertices[25] = vert1;
    vertices[26] = vert0;
    vertices[27] = vert4;
    vertices[28] = vert0;
    vertices[29] = vert5;
    // face down
    vertices[30] = vert6;
    vertices[31] = vert2;
    vertices[32] = vert3;
    vertices[33] = vert6;
    vertices[34] = vert7;
    vertices[35] = vert2;

    std::vector<float> vertexArray, normals;
    for(int i = 0; i < 36; ++i)
    {
        vertexArray.push_back(vertices[i].x);
        vertexArray.push_back(vertices[i].y);
        vertexArray.push_back(vertices[i].z);
    }

    for(int i = 0; i < 6; ++i)
    {
        glm::vec3 point0 = vertices[i * 6];
        glm::vec3 point1 = vertices[i * 6 + 1];
        glm::vec3 point2 = vertices[i * 6 + 2];
        glm::vec3 normal = glm::normalize(glm::cross(point0 - point1, point2 - point1));
        for(int j = 0; j < 6; ++j)
        {
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }

    return Loader::loadToVAO(vertexArray, 3, normals, 3);
}

// TODO: check params
// usage: Model.Add(name, color, points)
static int Lua_ModelAddCubeMesh(lua_State* L)
{
    if(!lua_isstring(L, -3))
    {
        printf("expected string, got %s\n", lua_typename(L, lua_type(L, -3)));
        return false;
    }

    const char* name = luaL_checkstring(L, -3);

    if(!lua_isinteger(L, -2))
    {
        printf("expected string, got %s\n", lua_typename(L, lua_type(L, -2)));
        return false;
    }

    const lua_Integer rgb = luaL_checkinteger(L, -2);
    const int r = (0xFF0000 & rgb) >> 16;
    const int g = (0x00FF00 & rgb) >> 8;
    const int b = (0x0000FF & rgb) >> 0;
    const glm::vec3 color(r / 255.f, g / 255.f, b / 255.f);

    if(!lua_istable(L, -1))
    {
        printf("expected table, got %s\n", lua_typename(L, lua_type(L, -1)));
        return false;
    }

    const size_t len = lua_rawlen(L, -1);
    assert(len == NUM_CUBE_VERTICES);

    std::vector<float> points;
    points.reserve(len);

    for(int index = 0; index < len; ++index)
    {
        // Our actual index will be +1 because Lua 1 indexes tables.
        int actualIndex = index + 1;
        // Push our target index to the stack.
        lua_pushinteger(L, actualIndex);
        // Get the table data at this index (and not get the table, which is what I thought this did.)
        lua_gettable(L, -2);
        // Check for the sentinel nil element.
        if(lua_type(L, -1) == LUA_TNIL)
            break;
        // Get it's value.
        points.push_back(static_cast<float>(luaL_checknumber(L, -1)));
        // Pop it off again.
        lua_pop(L, 1);
    }

    RawModel* model = createCube(points);

    Entity* entity = new Entity(model, glm::vec3(0), color, glm::vec3(1), 1.0f, false, false);
    Entity::addEntity(entity);

    return 0;
}

static const luaL_Reg s_modelFuncs[] = {
#define REG_FUNC(module, func)                                                                     \
    {                                                                                              \
#        func, Lua_##module##func                                                                  \
    }
    REG_FUNC(Model, AddCubeMesh),
#undef REG_FUNC
    {nullptr, nullptr},
};

bool ScriptManager::init()
{
    assert(m_state == nullptr);

    m_state = luaL_newstate();
    assert(m_state);
    if(m_state == nullptr)
    {
        return false;
    }

    luaL_openlibs(m_state);
    luaL_requiref(
        m_state,
        "Model",
        [](lua_State* state) {
            luaL_newlib(state, s_modelFuncs);
            return 1;
        },
        1);

    std::string srcFolder = buildLuaSrcFolder();
    std::string addPackagePath;

    char package[1024];
    snprintf(package,
             sizeof(package),
             "package.path = package.path .. ';' .. '%s/?.lua'",
             srcFolder.c_str());

    if(luaL_dostring(m_state, package) != LUA_OK)
    {
        reportError();
        return false;
    }

    return true;
}

void ScriptManager::deinit()
{
    if(m_state)
    {
        lua_close(m_state);
    }
}

bool ScriptManager::doFile(const char* file)
{
    assert(m_state);

    std::string fullpath = buildLuaSrcFolder();
    fullpath.append(file);

    if(luaL_dofile(m_state, fullpath.c_str()) != LUA_OK)
    {
        reportError();
        return false;
    }

    return true;
}

void ScriptManager::reportError()
{
    assert(m_state);

    lua_error(m_state);
    printf("*** lua error ***");
    printf("\n%s\n", lua_tostring(m_state, -1));
    printf("*****************");
}

std::string ScriptManager::buildLuaSrcFolder()
{
    char buffer[1024];
    strncpy(buffer, __FILE__, sizeof(buffer));
    char* slash = strrchr(buffer, '\\');
    if(slash)
    {
        *(slash + 1) = '\0';
    }
    std::string folder(buffer);
    folder.append("scripts/");
    std::replace(folder.begin(), folder.end(), '\\', '/');
    return folder;
}
