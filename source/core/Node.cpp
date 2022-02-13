#include "Node.h"
using glm::mat4;

mat4 Node::WorldTrans() const
{
    mat4 worldTrans = mTrans;
    for ( const Node* cursor = this; cursor->mParent; cursor = cursor->mParent )
    {
        worldTrans = cursor->mParent->mTrans * worldTrans;
    }

    return worldTrans;
}
