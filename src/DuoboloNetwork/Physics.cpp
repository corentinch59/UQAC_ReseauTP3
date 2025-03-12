//
// Created by theo on 11/03/2025.
//

#include <DuoBoloNetwork/Physics.h>

#include <raymath.h>

#define GRAVITY (-9.8f)

#define DEFAULT_DESTROY_DISTANCE_FROM_CENTER 100.0f

PhysicsSolver::PhysicsSolver() :
    mDefaultCollisionConfiguration(std::make_unique<btDefaultCollisionConfiguration>()),
    mCollisionDispatcher(std::make_unique<btCollisionDispatcher>(mDefaultCollisionConfiguration.get())),
    mOverlappingPairCache(std::make_unique<btDbvtBroadphase>()),
    mSolver(std::make_unique<btSequentialImpulseConstraintSolver>()),
    mWorld(std::make_unique<btDiscreteDynamicsWorld>(mCollisionDispatcher.get(), mOverlappingPairCache.get(),
                                                     mSolver.get(), mDefaultCollisionConfiguration.get()))
{
    mWorld->setGravity(btVector3(0, GRAVITY, 0));
}

PhysicsSolver::~PhysicsSolver() = default;

bool IsEqual(Vector3 rVec, btVector3 btVec)
{
    return rVec.x == btVec.getX() && rVec.y == btVec.getY() && rVec.z == btVec.getZ();
}

bool IsEqual(Quaternion rVec, btQuaternion btVec)
{
    return rVec.x == btVec.getX() && rVec.y == btVec.getY() && rVec.z == btVec.getZ() && rVec.w == btVec.getW();
}

void PhysicsSolver::Solve(entt::registry& world, float delta)
{
    auto view = world.view<Rigidbody, Transform>();

    // first destroy out of bounds entities
    for (auto&& [entity, physicComponent, transformComponent] : view.each())
    {
        if (Vector3LengthSqr(transformComponent.translation) > DEFAULT_DESTROY_DISTANCE_FROM_CENTER * DEFAULT_DESTROY_DISTANCE_FROM_CENTER)
        {
            if (physicComponent.mRigidbody->isInWorld())
                mWorld->removeRigidBody(physicComponent.mRigidbody.get());

            world.destroy(entity);
        }
    }

    for (auto&& [entity, physicComponent, transformComponent] : view.each())
    {
        if (!physicComponent.mRigidbody->isInWorld())
        {
            mWorld->addRigidBody(physicComponent.mRigidbody.get());
            spdlog::warn("Rigidbody not in world, adding...");
        }

        btTransform transform = physicComponent.mRigidbody->getWorldTransform();
        if (!IsEqual(transformComponent.translation, transform.getOrigin()))
        {
            spdlog::warn("Rigidbody position is unsynced!");
            transform.setOrigin({
                transformComponent.translation.x,
                transformComponent.translation.y,
                transformComponent.translation.z
            });

            physicComponent.mRigidbody->setWorldTransform(transform);
        }
        if (!IsEqual(transformComponent.rotation, transform.getRotation()))
        {
            spdlog::warn("Rigidbody rotation is unsynced!");
            transform.setRotation({
                transformComponent.rotation.x,
                transformComponent.rotation.y,
                transformComponent.rotation.z,
                transformComponent.rotation.w
            });

            physicComponent.mRigidbody->setWorldTransform(transform);
        }
    }

    mWorld->stepSimulation(delta, 10);

    for (auto&& [entity, physicComponent, transformComponent] : view.each())
    {
        btTransform transform = physicComponent.mRigidbody->getWorldTransform();

        auto position = transform.getOrigin();
        transformComponent.translation = {position.getX(), position.getY(), position.getZ()};
        auto rotation = transform.getRotation();
        transformComponent.rotation = {rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW()};
    }
}
