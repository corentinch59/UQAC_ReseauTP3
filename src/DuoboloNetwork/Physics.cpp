//
// Created by theo on 11/03/2025.
//

#include <DuoBoloNetwork/Physics.h>

#include <raymath.h>

#include <DuoBoloNetwork/Helpers.h>
#include <DuoBoloNetwork/Transform.h>

#define GRAVITY (-9.8f)

#define DEFAULT_DESTROY_DISTANCE_FROM_CENTER 100.0f

PhysicsSolver::PhysicsSolver(entt::registry& world) : mEnttWorld(world),
                                                      mDefaultCollisionConfiguration(
                                                          std::make_unique<btDefaultCollisionConfiguration>()),
                                                      mCollisionDispatcher(
                                                          std::make_unique<btCollisionDispatcher>(
                                                              mDefaultCollisionConfiguration.get())),
                                                      mOverlappingPairCache(std::make_unique<btDbvtBroadphase>()),
                                                      mSolver(std::make_unique<btSequentialImpulseConstraintSolver>()),
                                                      mWorld(std::make_unique<btDiscreteDynamicsWorld>(
                                                          mCollisionDispatcher.get(), mOverlappingPairCache.get(),
                                                          mSolver.get(), mDefaultCollisionConfiguration.get()))
{
    mWorld->setGravity(btVector3(0, GRAVITY, 0));

    mEnttWorld.on_construct<RigidbodyComponent>().connect<&PhysicsSolver::HandleRigidbodyCreate>(this);
    mEnttWorld.on_destroy<RigidbodyComponent>().connect<&PhysicsSolver::HandleRigidbodyDestroy>(this);
}

PhysicsSolver::~PhysicsSolver() = default;

void PhysicsSolver::SyncRigidbodyAndTransform(const entt::entity& entity, RigidbodyComponent& rbComp, TransformComponent& tComp)
{
    if (!IsEqual(rbComp.velocity, mRigidbodies[entity]->btRigidBody::getLinearVelocity()))
    {
        spdlog::warn("Rigidbody velocity is unsynced!");
        mRigidbodies[entity]->btRigidBody::setLinearVelocity(ToBtVector3(rbComp.velocity));
    }

    btTransform transform = mRigidbodies[entity]->btCollisionObject::getWorldTransform();
    if (!IsEqual(tComp.position, transform.getOrigin()))
    {
        spdlog::warn("Rigidbody position is unsynced!");
        transform.setOrigin({
            tComp.position.x,
            tComp.position.y,
            tComp.position.z
        });

        mRigidbodies[entity]->btCollisionObject::setWorldTransform(transform);
    }
    if (!IsEqual(tComp.rotation, transform.getRotation()))
    {
        spdlog::warn("Rigidbody rotation is unsynced!");
        transform.setRotation({
            tComp.rotation.x,
            tComp.rotation.y,
            tComp.rotation.z,
            tComp.rotation.w
        });

        mRigidbodies[entity]->btCollisionObject::setWorldTransform(transform);
    }
}

void PhysicsSolver::Solve(float delta)
{
    auto view = mEnttWorld.view<RigidbodyComponent, TransformComponent>();

    for (auto&& [entity, rigidbodyComponent, transformComponent] : view.each())
    {
        // update velocity if requested
        SyncRigidbodyAndTransform(entity, rigidbodyComponent, transformComponent);
    }

    mWorld->stepSimulation(delta, 10);

    for (auto&& [entity, rigidbodyComponent, transformComponent] : view.each())
    {
        if (!mRigidbodies.contains(entity)) continue;

        btTransform transform = mRigidbodies[entity]->getWorldTransform();

        auto position = transform.getOrigin();
        transformComponent.position = {position.getX(), position.getY(), position.getZ()};
        auto rotation = transform.getRotation();
        transformComponent.rotation = {rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW()};
        rigidbodyComponent.velocity = ToVector3(mRigidbodies[entity]->getLinearVelocity());
    }
}

std::unique_ptr<btCollisionShape> CreateShapeFromAny(entt::any shape)
{
    if (shape.type().index() == entt::type_index<BoxShape>())
    {
        auto boxShape = entt::any_cast<BoxShape>(shape);
        return std::make_unique<btBoxShape>(ToBtVector3(boxShape.extent/2.0f));
    }

    if (shape.type().index() == entt::type_index<SphereShape>())
    {
        auto sphereShape = entt::any_cast<SphereShape>(shape);
        return std::make_unique<btSphereShape>(sphereShape.radius);
    }

    return nullptr;
}


void PhysicsSolver::HandleRigidbodyCreate(entt::registry& registry, entt::entity entity)
{
    RigidbodyComponent& comp = registry.get<RigidbodyComponent>(entity);

    mCollisionShapes[entity] = CreateShapeFromAny(comp.shape);

    btVector3 bInertia(0.0f, 0.0f, 0.0f);
    if (comp.mass != 0.0f) mCollisionShapes[entity]->calculateLocalInertia(comp.mass, bInertia);

    mMotionStates[entity] = std::make_unique<btDefaultMotionState>();
    mRigidbodies[entity] = std::make_unique<btRigidBody>(
        comp.mass,
        mMotionStates[entity].get(),
        mCollisionShapes[entity].get(),
        bInertia
    );
    mWorld->addRigidBody(mRigidbodies[entity].get());
}

void PhysicsSolver::HandleRigidbodyDestroy(entt::registry& registry, entt::entity entity)
{
    auto& comp = registry.get<RigidbodyComponent>(entity);

    if (mRigidbodies.contains(entity))
    {
        mWorld->removeRigidBody(mRigidbodies[entity].get());
        mRigidbodies.erase(entity);
        mMotionStates.erase(entity);
        mCollisionShapes.erase(entity);
    }
}
