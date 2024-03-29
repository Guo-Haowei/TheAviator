#include "Geometry.h"

#include <cassert>

using std::array;
using std::vector;

Mesh& Mesh::MakeBox( const vec3& color, float size, float alpha )
{
    const std::array<vec3, 8> points = {
        vec3( +size, +size, +size ),
        vec3( +size, +size, -size ),
        vec3( +size, -size, +size ),
        vec3( +size, -size, -size ),
        vec3( -size, +size, -size ),
        vec3( -size, +size, +size ),
        vec3( -size, -size, -size ),
        vec3( -size, -size, +size ),
    };
    return MakeBox( points, color, alpha );
}

Mesh& Mesh::MakeBox( const array<vec3, 8>& points, const vec3& color, float alpha )
{
    mColor = vec4( color, alpha );

    const vec3 vert0 = points[0];
    const vec3 vert1 = points[1];
    const vec3 vert2 = points[2];
    const vec3 vert3 = points[3];
    const vec3 vert4 = points[4];
    const vec3 vert5 = points[5];
    const vec3 vert6 = points[6];
    const vec3 vert7 = points[7];

    // clang-format off
    mPositions = {
        // left
        vert0, vert3, vert1, vert0, vert2, vert3,
        // right
        vert4, vert6, vert5, vert5, vert6, vert7,
        // front
        vert0, vert7, vert2, vert0, vert5, vert7,
        // back
        vert1, vert3, vert6, vert1, vert6, vert4,
        // up
        vert1, vert4, vert0, vert0, vert4, vert5,
        // down
        vert2, vert6, vert3, vert6, vert2, vert7,
    };
    // clang-format on

    mFaces.clear();
    for ( int i = 0; i < 36; i += 3 )
    {
        mFaces.push_back( uvec3( i, i + 1, i + 2 ) );
    }

    return *this;
}

Mesh& Mesh::MakeCylinder( const vec3& color, float radius, float heightPerSeg, int radialSegments, int heightSegments, float alpha )
{
    assert( radialSegments > 0 );
    assert( heightSegments > 0 );

    mColor = vec4( color, alpha );
    mPositions.clear();
    mFaces.clear();

    for ( int i = 0; i < heightSegments; ++i )
    {
        for ( int j = 0; j < radialSegments; ++j )
        {
            const float angle1 = ( static_cast<float>( j ) / radialSegments ) * ( 2 * glm::pi<float>() );
            const float angle2 = ( static_cast<float>( j + 1 ) / radialSegments ) * ( 2 * glm::pi<float>() );
            const float y1 = ( i - heightSegments * 0.5f ) * heightPerSeg;
            const float y2 = ( i - heightSegments * 0.5f ) * heightPerSeg + heightPerSeg;
            const vec3 p1( glm::sin( angle1 ) * radius, y1, glm::cos( angle1 ) * radius );
            const vec3 p2( glm::sin( angle2 ) * radius, y1, glm::cos( angle2 ) * radius );
            const vec3 p3( glm::sin( angle1 ) * radius, y2, glm::cos( angle1 ) * radius );
            const vec3 p4( glm::sin( angle2 ) * radius, y2, glm::cos( angle2 ) * radius );

            mPositions.push_back( p1 );
            mPositions.push_back( p2 );
            mPositions.push_back( p3 );

            mPositions.push_back( p2 );
            mPositions.push_back( p4 );
            mPositions.push_back( p3 );
        }
    }

    return *this;
}

void Mesh::ApplyMatrix( const mat4& trans )
{
    mTrans = trans * mTrans;
}

Mesh& Mesh::Scale( float x, float y, float z )
{
    mTrans = glm::scale( mat4( 1.0f ), vec3( x, y, z ) ) * mTrans;
    return *this;
}

Mesh& Mesh::Translate( float x, float y, float z )
{
    mTrans = glm::translate( mat4( 1.0f ), vec3( x, y, z ) ) * mTrans;
    return *this;
}

Mesh& Mesh::RotateX( float degree )
{
    mTrans = glm::rotate( mat4( 1.0f ), glm::radians( degree ), vec3( 1, 0, 0 ) ) * mTrans;
    return *this;
}

Mesh& Mesh::RotateY( float degree )
{
    mTrans = glm::rotate( mat4( 1.0f ), glm::radians( degree ), vec3( 0, 1, 0 ) ) * mTrans;
    return *this;
}

Mesh& Mesh::RotateZ( float degree )
{
    mTrans = glm::rotate( mat4( 1.0f ), glm::radians( degree ), vec3( 0, 0, 1 ) ) * mTrans;
    return *this;
}

void MeshGroup::BuildBuffers( std::vector<Vertex>& outVertices ) const
{
    outVertices.clear();
    for ( const Mesh& mesh : mMeshes )
    {
        const mat4& M = mesh.mTrans;
        if ( mesh.mFaces.size() )
        {
            for ( const uvec3& face : mesh.mFaces )
            {
                const vec4 p0 = M * vec4( mesh.mPositions[face.x], 1.0f );
                const vec4 p1 = M * vec4( mesh.mPositions[face.y], 1.0f );
                const vec4 p2 = M * vec4( mesh.mPositions[face.z], 1.0f );
                const vec3 normal = glm::normalize( glm::cross( vec3( p2 - p1 ), vec3( p0 - p1 ) ) );

                Vertex vertex;
                vertex.color = mesh.mColor;
                vertex.normal = normal;
                vertex.position = p0;
                outVertices.push_back( vertex );
                vertex.position = p1;
                outVertices.push_back( vertex );
                vertex.position = p2;
                outVertices.push_back( vertex );
            }
        }
        else
        {
            for ( size_t i = 0; i < mesh.mPositions.size(); i += 3 )
            {
                const vec4 p0 = M * vec4( mesh.mPositions.at( i + 0 ), 1.0f );
                const vec4 p1 = M * vec4( mesh.mPositions.at( i + 1 ), 1.0f );
                const vec4 p2 = M * vec4( mesh.mPositions.at( i + 2 ), 1.0f );
                const vec3 normal = glm::normalize( glm::cross( vec3( p2 - p1 ), vec3( p0 - p1 ) ) );

                Vertex vertex;
                vertex.color = mesh.mColor;
                vertex.normal = normal;
                vertex.position = p0;
                outVertices.push_back( vertex );
                vertex.position = p1;
                outVertices.push_back( vertex );
                vertex.position = p2;
                outVertices.push_back( vertex );
            }
        }
    }
}
