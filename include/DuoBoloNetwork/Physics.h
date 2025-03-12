//
// Created by theo on 11/03/2025.
//

#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <raylib.h>

#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

struct Rigidbody
{
    std::unique_ptr<btCollisionShape> mShape;
    std::unique_ptr<btMotionState> mMotionState;
    std::unique_ptr<btRigidBody> mRigidbody;

    // Box rigidbody constructor
    explicit Rigidbody(float mass, Vector3 boxExtent)
    {
        btVector3 extent = {boxExtent.x, boxExtent.y, boxExtent.z};
        mShape = std::make_unique<btBoxShape>(extent / 2.0f);
        mMotionState = std::make_unique<btDefaultMotionState>();

        btVector3 bInertia(0.0f, 0.0f, 0.0f);
        if (mass != 0.0f)
            mShape->calculateLocalInertia(mass, bInertia);

        mRigidbody = std::make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo{
            mass,
            mMotionState.get(),
            mShape.get(),
            bInertia
        });
    }

    explicit Rigidbody(float mass, float radius)
    {
        mShape = std::make_unique<btSphereShape>(radius);
        mMotionState = std::make_unique<btDefaultMotionState>();

        btVector3 bInertia(0.0f, 0.0f, 0.0f);
        if (mass != 0.0f)
            mShape->calculateLocalInertia(mass, bInertia);

        mRigidbody = std::make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo{
            mass,
            mMotionState.get(),
            mShape.get(),
            bInertia
        });
    }
};

class PhysicsSolver
{
public:
    PhysicsSolver();
    ~PhysicsSolver();

    void OnTransformManuallyModified(Rigidbody& rb, Transform& transform);

    void Solve(entt::registry& world, float delta);

private:
    std::unique_ptr<btDefaultCollisionConfiguration> mDefaultCollisionConfiguration;
    std::unique_ptr<btCollisionDispatcher> mCollisionDispatcher;
    std::unique_ptr<btBroadphaseInterface> mOverlappingPairCache;
    std::unique_ptr<btConstraintSolver> mSolver;
    std::unique_ptr<btDynamicsWorld> mWorld;
};
