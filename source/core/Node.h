#pragma once
#include <memory>
#include <string>
#include <vector>

#include "MathHelper.h"

class Node {
   public:
    using Ptr = std::shared_ptr<Node>;
    using NodeList = std::vector<Ptr>;

    Node( const char* name )
        : mName( name )
        , mParent( nullptr )
    {
        mTrans = mat4( 1.0f );
    }

    void SetMeshKey( const char* meshKey )
    {
        mMeshKey = std::string( meshKey );
    }

    void AppendChild( Ptr child )
    {
        mChildren.push_back( child );
        child->mParent = this;
    }

    mat4 WorldTrans() const;

    inline const mat4& Trans() const
    {
        return mTrans;
    }

   protected:
    std::string mName;
    std::string mMeshKey;
    mat4 mTrans;
    NodeList mChildren;
    Node* mParent;
};
