//
// Created by tinti on 11/03/2025.
//

#pragma once

#include <btBulletDynamicsCommon.h>

#include <Corrade/Containers/Pointer.h>

#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/SceneGraph.h>
#include <Magnum/Math/Color.h>

typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> Object3D;

using namespace Magnum::Math::Literals;

class RigidBody : public Object3D
{
    public:
        RigidBody(Object3D* parent, Magnum::Float mass, btCollisionShape* bShape, btDynamicsWorld& bWorld);

        ~RigidBody();

        btRigidBody& rigidBody() { return *_bRigidBody; }

        void syncPose();

    private:
        btDynamicsWorld& _bWorld;
        Magnum::Containers::Pointer<btRigidBody> _bRigidBody;
};
