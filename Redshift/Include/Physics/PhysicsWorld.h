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
    , mDynamicWorld{ new btDiscreteDynamicsWorld{ mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration } }
  {
    
  }
  virtual ~PhysicsWorld()
  {
    delete mDynamicWorld;
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
  btDiscreteDynamicsWorld* mDynamicWorld;
};