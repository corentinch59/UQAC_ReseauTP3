//
// Created by tinti on 11/03/2025.
//

#include <RigidBody.h>

#include <Magnum/BulletIntegration/Integration.h>
#include <Magnum/BulletIntegration/MotionState.h>

typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> Object3D;
using namespace Magnum::Math::Literals;

RigidBody::RigidBody(Object3D* parent, Magnum::Float mass, btCollisionShape* bShape, btDynamicsWorld& bWorld):
Object3D{parent},
_bWorld(bWorld)
{
    btVector3 bInertia(0.0f, 0.0f, 0.0f);
    if(!Magnum::Math::TypeTraits<Magnum::Float>::equals(mass, 0.0f))
        bShape->calculateLocalInertia(mass, bInertia);

    auto* motionState = new Magnum::BulletIntegration::MotionState{*this};
    _bRigidBody.emplace(btRigidBody::btRigidBodyConstructionInfo{
        mass, &motionState->btMotionState(), bShape, bInertia});
    _bRigidBody->forceActivationState(DISABLE_DEACTIVATION);
    bWorld.addRigidBody(_bRigidBody.get());
}

RigidBody::~RigidBody()
{
    _bWorld.removeRigidBody(_bRigidBody.get());
}

void RigidBody::syncPose() {
    _bRigidBody->setWorldTransform(btTransform(transformationMatrix()));
}