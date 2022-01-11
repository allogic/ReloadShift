#pragma once

#include <Core.h>

class PhysicsWorld
{
public:

  PhysicsWorld()
    : mCollisionConfiguration{ new btDefaultCollisionConfiguration }
    , mDispatcher{ new btCollisionDispatcher{ mCollisionConfiguration } }
    , mOverlappingPairCache{ new btDbvtBroadphase }
    , mSolver{ new btSequentialImpulseConstraintSolver }
    , mDynamicsWorld{ new btDiscreteDynamicsWorld{ mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration } }
  {
    mDynamicsWorld->setGravity(btVector3(0, -10, 0));
  }
  virtual ~PhysicsWorld()
  {
    delete mDynamicsWorld;
    delete mSolver;
    delete mOverlappingPairCache;
    delete mDispatcher;
    delete mCollisionConfiguration;
  }

private:

  btDefaultCollisionConfiguration* mCollisionConfiguration;
  btCollisionDispatcher* mDispatcher;
  btBroadphaseInterface* mOverlappingPairCache;
  btSequentialImpulseConstraintSolver* mSolver;
  btDiscreteDynamicsWorld* mDynamicsWorld;

  btAlignedObjectArray<btCollisionShape*> mCollisionSHapes;
};