//
// Created by theo on 11/03/2025.
//

#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <raylib.h>

#include <DuoBoloNetwork/Transform.h>

#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

#include <entt/entt.hpp>

struct BoxShape
{
    Vector3 extent;
};

struct SphereShape
{
    float radius;
};

struct RigidbodyComponent
{
    float mass = 1.f;
    entt::any shape = BoxShape{{1, 1, 1}};
    Vector3 velocity = {0, 0, 0};
};

class PhysicsSolver
{
public:
    explicit PhysicsSolver(entt::registry& world);
    ~PhysicsSolver();

    void SyncRigidbodyAndTransform(const entt::entity& entity, RigidbodyComponent& rbComp, TransformComponent& tComp);

    void Solve(float delta);

private:
    entt::registry& mEnttWorld;

    std::unique_ptr<btDefaultCollisionConfiguration> mDefaultCollisionConfiguration;
    std::unique_ptr<btCollisionDispatcher> mCollisionDispatcher;
    std::unique_ptr<btBroadphaseInterface> mOverlappingPairCache;
    std::unique_ptr<btConstraintSolver> mSolver;
    std::unique_ptr<btDynamicsWorld> mWorld;
    uint32_t mNextRbId = 0;
    entt::dense_map<entt::entity, std::unique_ptr<btCollisionShape>> mCollisionShapes;
    entt::dense_map<entt::entity, std::unique_ptr<btRigidBody>> mRigidbodies;
    entt::dense_map<entt::entity, std::unique_ptr<btMotionState>> mMotionStates;
    void HandleRigidbodyCreate(entt::registry& registry, entt::entity entity);
    void HandleRigidbodyDestroy(entt::registry& registry, entt::entity entity);
};
