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

    template<class Shape, typename... Args>
    static Rigidbody Create(float mass, Args ...args)
    {
        Rigidbody rb{};
        rb.mShape = std::make_unique<Shape>(std::forward<Args>(args)...);
        rb.mMotionState = std::make_unique<btDefaultMotionState>();

        btVector3 bInertia(0.0f, 0.0f, 0.0f);
        if(mass != 0.0f)
            rb.mShape->calculateLocalInertia(mass, bInertia);

        rb.mRigidbody = std::make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo{
            mass,
            rb.mMotionState.get(),
            rb.mShape.get(),
            bInertia
        });
        return rb;
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