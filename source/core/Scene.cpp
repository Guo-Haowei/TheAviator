#include "Scene.h"

#include <string>
#include <unordered_map>

using glm::mat4;
using glm::uvec3;
using glm::vec3;
using glm::vec4;
using std::array;
using std::string;
using std::unordered_map;
using std::vector;

static unordered_map<string, MeshGroup> s_meshes;
Node g_scene( SCENE_KEY_ROOT );

static void BuildAirplane();

void InitMeshes()
{
    BuildAirplane();
}

MeshGroup* FindMesh( const char* key )
{
    auto it = s_meshes.find( string( key ) );
    if ( it == s_meshes.end() )
    {
        return nullptr;
    }

    return &it->second;
}

void SetupScene()
{
    Node::Ptr airplane = std::make_unique<Node>( SCENE_KEY_AIRPLANE );
    Node::Ptr airplanePropeller = std::make_unique<Node>( SCENE_KEY_AIRPLANE_PROPELLER );
    airplanePropeller->SetMeshKey( MESH_KEY_AIRPLANE_PROPELLER );
    Node::Ptr airplaneBody = std::make_unique<Node>( SCENE_KEY_AIRPLANE_BODY );
    airplaneBody->SetMeshKey( MESH_KEY_AIRPLANE_BODY );

    airplane->AppendChild( airplaneBody );
    airplane->AppendChild( airplanePropeller );
    g_scene.AppendChild( airplane );
}

static void BuildAirplane()
{
    using namespace color;

    const std::array<vec3, 8> cockpitPoints = {
        vec3( 4, 2.5, 2.5 ),
        vec3( 4, 2.5, -2.5 ),
        vec3( 4, -2.5, 2.5 ),
        vec3( 4, -2.5, -2.5 ),
        vec3( -4, 1.5, -0.5 ),
        vec3( -4, 1.5, 0.5 ),
        vec3( -4, 0.5, -0.5 ),
        vec3( -4, 0.5, 0.5 )
    };
    const std::array<vec3, 8> propellerPoints = {
        vec3( 0.5, 0.5, 0.5 ),
        vec3( 0.5, 0.5, -0.5 ),
        vec3( 0.5, -0.5, 0.5 ),
        vec3( 0.5, -0.5, -0.5 ),
        vec3( -0.5, 0, 0 ),
        vec3( -0.5, 0, 0 ),
        vec3( -0.5, 0, 0 ),
        vec3( -0.5, 0, 0 ),
    };

    {
        Mesh mesh;
        MeshGroup airplaneBody;
#define ADD_AND_RESET_MESH               \
    {                                    \
        airplaneBody.AddSubMesh( mesh ); \
        mesh.Reset();                    \
    }
        // cockpit
        mesh.MakeBox( cockpitPoints, RED, 1.0f );
        ADD_AND_RESET_MESH;
        // engine
        mesh.MakeBox( WHITE ).Scale( 2.0f, 5.0f, 5.0f ).Translate( 5.f, 0.0f, 0.0f );
        ADD_AND_RESET_MESH;
        // sideWing
        mesh.MakeBox( RED ).Scale( 3.0f, 0.5f, 12.0f ).Translate( 0.0f, 1.5f, 0.0f );
        ADD_AND_RESET_MESH;
        // tail
        mesh.MakeBox( RED ).Scale( 1.5f, 2.0f, 0.5f ).Translate( -4.0f, 2.0f, 0.0f );
        ADD_AND_RESET_MESH;
        // wind shield
        mesh.MakeBox( WHITE, DEFAULT_MESH_SIZE, 0.3f ).Scale( 0.3f, 1.5f, 2.0f ).Translate( 0.5f, 2.7f, 0.0f );
        ADD_AND_RESET_MESH;
        // wheel protection 1
        mesh.MakeBox( RED ).Scale( 3.0f, 1.5f, 1.0f ).Translate( 2.5f, -2.0f, 2.5f );
        ADD_AND_RESET_MESH;
        // wheel protection 2
        mesh.MakeBox( RED ).Scale( 3.0f, 1.5f, 1.0f ).Translate( 2.5f, -2.0f, -2.5f );
        ADD_AND_RESET_MESH;
        // tire 1
        mesh.MakeBox( DARK_BROWN ).Scale( 2.4f, 2.4f, 0.4f ).Translate( 2.5f, -2.8f, 2.5f );
        ADD_AND_RESET_MESH;
        // tire 2
        mesh.MakeBox( DARK_BROWN ).Scale( 2.4f, 2.4f, 0.4f ).Translate( 2.5f, -2.8f, -2.5f );
        ADD_AND_RESET_MESH;
        //  wheelAxis
        mesh.MakeBox( BROWN ).Scale( 1.0f, 1.0f, 5.9f ).Translate( 2.5, -2.8f, 0.0f );
        ADD_AND_RESET_MESH;
        // suspension
        mesh.MakeBox( RED )
            .Scale( 0.4f, 2.0f, 0.4f )
            .RotateZ( 160.0f )
            .Translate( -3.3f, 0.2f, 0.0f );
        ADD_AND_RESET_MESH;
        // tire
        mesh.MakeBox( DARK_BROWN )
            .Scale( 0.8f, 0.8f, 0.3f )
            .Translate( -3.5f, -0.8f, 0.0f );
        ADD_AND_RESET_MESH;
        mesh.MakeBox( DARK_BROWN )
            .Scale( 1.2f, 1.2f, 0.2f )
            .Translate( -3.5f, -0.8f, 0.0f );
        ADD_AND_RESET_MESH;
#undef ADD_AND_RESET_MESH
        s_meshes[MESH_KEY_AIRPLANE_BODY] = airplaneBody;
    }

    {
        Mesh mesh;
        MeshGroup airplanePropeller;
#define ADD_AND_RESET_MESH                    \
    {                                         \
        airplanePropeller.AddSubMesh( mesh ); \
        mesh.Reset();                         \
    }
        // propeller
        mesh.MakeBox( propellerPoints, RED, 1.0f ).Scale( 2.0f, 1.0f, 1.0f ).Translate( 6.0f, 0.0f, 0.0f );
        ADD_AND_RESET_MESH;
        // blade 1
        mesh.MakeBox( DARK_BROWN ).Scale( 0.1f, 8.0f, 1.0f ).Translate( 6.8f, 0.0f, 0.0f );
        ADD_AND_RESET_MESH;
        mesh.MakeBox( DARK_BROWN )
            .Scale( 0.1f, 8.0f, 1.0f )
            .RotateX( 90.0f )
            .Translate( 6.8f, 0.0f, 0.0f );
        ADD_AND_RESET_MESH;
#undef ADD_AND_RESET_MESH
        s_meshes[MESH_KEY_AIRPLANE_PROPELLER] = airplanePropeller;
    }
}