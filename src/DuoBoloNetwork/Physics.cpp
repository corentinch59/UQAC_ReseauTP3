//
// Created by theo on 11/03/2025.
//

#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Helpers.h>
#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/PhysicsComponent.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <raymath.h>

#include <spdlog/spdlog.h>

#include "DuoBoloShared/UserCreationInfoComponent.h"

#define GRAVITY (-9.8f)

#define DEFAULT_DESTROY_DISTANCE_FROM_CENTER 100.0f

PhysicsSolver* PhysicsSolver::gPhysicsSolver = nullptr;

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
	if (gPhysicsSolver)
	{
		spdlog::error("A physics solver already exists!");
		exit(1);
	}

	gPhysicsSolver = this;

	mCollisionDispatcher->setNearCallback(NearCallback);

	mWorld->setGravity(btVector3(0, GRAVITY, 0));

	mEnttWorld.on_construct<RigidbodyComponent>().connect<&PhysicsSolver::HandleRigidbodyCreate>(this);
	mEnttWorld.on_destroy<RigidbodyComponent>().connect<&PhysicsSolver::HandleRigidbodyDestroy>(this);
}

PhysicsSolver::~PhysicsSolver()
{
	if (gPhysicsSolver == this) gPhysicsSolver = nullptr;
};

void PhysicsSolver::SyncRigidbodyAndTransform(const entt::entity& entity, RigidbodyComponent& rbComp,
                                              TransformComponent& tComp)
{
	if (!IsEqual(rbComp.velocity, mRigidbodies[entity]->getLinearVelocity()))
	{
		// spdlog::warn("Rigidbody velocity is unsynced!");
		mRigidbodies[entity]->btRigidBody::setLinearVelocity(ToBtVector3(rbComp.velocity));
	}

	btTransform transform = mRigidbodies[entity]->btCollisionObject::getWorldTransform();
	if (!IsEqual(tComp.position, transform.getOrigin()))
	{
		// spdlog::warn("Rigidbody position is unsynced!");
		transform.setOrigin({
			tComp.position.x,
			tComp.position.y,
			tComp.position.z
		});

		mRigidbodies[entity]->setWorldTransform(transform);
	}
	if (!IsEqual(tComp.rotation, transform.getRotation()))
	{
		// spdlog::warn("Rigidbody rotation is unsynced!");
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

	mWorld->stepSimulation(delta, 10, 1.0f / 120.f);

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
		return std::make_unique<btBoxShape>(ToBtVector3(boxShape.extent / 2.0f));
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
	if (comp.mass != 0.0f && mCollisionShapes[entity])
		mCollisionShapes[entity]->calculateLocalInertia(
			comp.mass, bInertia);

	mMotionStates[entity] = std::make_unique<btDefaultMotionState>();
	mRigidbodies[entity] = std::make_unique<btRigidBody>(
		comp.mass,
		mMotionStates[entity].get(),
		mCollisionShapes[entity].get(),
		bInertia
	);
	mRigidbodies[entity]->setUserIndex(static_cast<int>(entity));
	mWorld->addRigidBody(mRigidbodies[entity].get());
}

void PhysicsSolver::HandleRigidbodyDestroy(entt::registry& registry, entt::entity entity)
{
	if (mRigidbodies.contains(entity))
	{
		mWorld->removeRigidBody(mRigidbodies[entity].get());
		mRigidbodies.erase(entity);
		mMotionStates.erase(entity);
		mCollisionShapes.erase(entity);
	}
}

void PhysicsSolver::NearCallback(btBroadphasePair& collisionPair,
                                 btCollisionDispatcher& dispatcher,
                                 const btDispatcherInfo& dispatchInfo)
{
	// Default processing (generates contact points)
	btCollisionObject* colObj0 = (btCollisionObject*)collisionPair.m_pProxy0->m_clientObject;
	btCollisionObject* colObj1 = (btCollisionObject*)collisionPair.m_pProxy1->m_clientObject;

	// You can filter collisions here if needed
	dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);

	// Example: Only trigger on rigid bodies
	btRigidBody* body0 = btRigidBody::upcast(colObj0);
	btRigidBody* body1 = btRigidBody::upcast(colObj1);
	if (body0 && body1)
	{
		if (gPhysicsSolver) gPhysicsSolver->OnCollision(body0, body1);
	}
}

void PhysicsSolver::OnCollision(btRigidBody* body0, btRigidBody* body1)
{
	entt::entity entity0 = static_cast<entt::entity>(body0->getUserIndex());
	entt::entity entity1 = static_cast<entt::entity>(body1->getUserIndex());

	if (mEnttWorld.any_of<UserCreationInfoComponent>(entity0))
	{
		auto& userComp = mEnttWorld.get<UserCreationInfoComponent>(entity0);
		auto& rbComp = mEnttWorld.get<RigidbodyComponent>(entity1);
		rbComp.lastTouchedByUser = userComp.connectionId;
	}

	if (mEnttWorld.any_of<UserCreationInfoComponent>(entity1))
	{
		auto& userComp = mEnttWorld.get<UserCreationInfoComponent>(entity1);
		auto& rbComp = mEnttWorld.get<RigidbodyComponent>(entity0);
		rbComp.lastTouchedByUser = userComp.connectionId;
	}
}
