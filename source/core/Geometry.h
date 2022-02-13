#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>

inline constexpr float DEFAULT_MESH_SIZE = 0.5f;

namespace color {

inline constexpr glm::vec3 MakeColorFromHex( uint32_t hex )
{
    const float r = static_cast<float>( ( hex & 0xFF0000 ) >> 16 );
    const float g = static_cast<float>( ( hex & 0x00FF00 ) >> 8 );
    const float b = static_cast<float>( ( hex & 0x0000FF ) );
    return glm::vec3{ r / 255.f, g / 255.f, b / 255.f };
}

inline constexpr glm::vec3 RED = MakeColorFromHex( 0xF25346 );
inline constexpr glm::vec3 WHITE = MakeColorFromHex( 0xD8D0D1 );
inline constexpr glm::vec3 BROWN = MakeColorFromHex( 0x59332e );
inline constexpr glm::vec3 PINK = MakeColorFromHex( 0xF5986E );
inline constexpr glm::vec3 DARK_BROWN = MakeColorFromHex( 0x23190F );
inline constexpr glm::vec3 BLUE = MakeColorFromHex( 0x68C3C0 );

};  // namespace color

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
};

struct Mesh {
    Mesh& MakeBox( const glm::vec3& color, float size = DEFAULT_MESH_SIZE, float alpha = 1.0f );
    Mesh& MakeBox( const std::array<glm::vec3, 8>& points, const glm::vec3& color, float alpha );

    Mesh& Scale( float x, float y, float z );
    Mesh& Translate( float x, float y, float z );
    Mesh& RotateX( float degree );
    Mesh& RotateY( float degree );
    Mesh& RotateZ( float degree );

    Mesh()
    {
        Reset();
    }

    void Reset()
    {
        mPositions.clear();
        mFaces.clear();
        mColor = glm::vec4( 1.0f );
        mTrans = glm::mat4( 1.0f );
    }

   private:
    void ApplyMatrix( const glm::mat4& trans );

    std::vector<glm::vec3> mPositions;
    std::vector<glm::uvec3> mFaces;
    glm::vec4 mColor;
    glm::mat4 mTrans;

    friend class MeshGroup;
};

class MeshGroup {
   public:
    void BuildBuffers( std::vector<Vertex>& outVertices, std::vector<glm::uvec3>& outFaces ) const;
    inline void AddSubMesh( const Mesh& mesh ) { mMeshes.push_back( mesh ); }

   protected:
    std::vector<Mesh> mMeshes;
    std::string mName;
};
