#include <spdlog/spdlog.h>

#include <raylib.h>
#include <raymath.h>

#include <entt/entt.hpp>

#include "DuoBoloNetwork/Physics.h"
#include "DuoBoloNetwork/Rendering.h"

int main()
{
    spdlog::info("Hello!");

    InitWindow(1280, 720, "DuoBolo TP3");
    SetTargetFPS(60);

    entt::registry world{};

    Camera camera{};
    camera.position = {0.0f, 2.0f, 4.0f}; // Camera position
    camera.target = {0.0f, 0.0f, 0.0f}; // Camera looking at point
    camera.up = {0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
    camera.fovy = 60.0f; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;

    Renderer renderer;
    PhysicsSolver solver;

    Model cube = LoadModelFromMesh(GenMeshCube(1, 1, 1));
    renderer.UpdateMeshMaterialsToUseCorrectShader(cube);
    Model plane = LoadModelFromMesh(GenMeshPlane(10, 10, 1, 1));
    renderer.UpdateMeshMaterialsToUseCorrectShader(plane);

    auto cubeEntity = world.create();
    auto& cubeTransform = world.emplace_or_replace<Transform>(cubeEntity);
    cubeTransform.scale = {1, 1, 1};
    cubeTransform.translation = {0, 10, 0};
    cubeTransform.rotation = QuaternionIdentity();

    auto& cubeRenderable = world.emplace_or_replace<Renderable>(cubeEntity);
    cubeRenderable.model = cube;

    auto& cubeRigidbody = world.emplace_or_replace<Rigidbody>(cubeEntity);
    btVector3 cubeShapeExtent{.5f, .5f, .5f};
    cubeRigidbody = Rigidbody::Create<btBoxShape>(1.f, cubeShapeExtent);

    auto planeEntity = world.create();
    auto& planeTransform = world.emplace_or_replace<Transform>(planeEntity);
    planeTransform.scale = {1, 1, 1};
    planeTransform.translation = {0, 0, 0};
    planeTransform.rotation = QuaternionIdentity();

    auto& planeRenderable = world.emplace_or_replace<Renderable>(planeEntity);
    planeRenderable.model = plane;

    auto& planeRigidbody = world.emplace_or_replace<Rigidbody>(planeEntity);
    btVector3 planeNormal{0, 1, 0};
    planeRigidbody = Rigidbody::Create<btStaticPlaneShape>(0, planeNormal, 0.01f);


    float lastPrintTime = GetTime();

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_ORBITAL);

        if (GetTime() - lastPrintTime > 1.0f)
        {
            lastPrintTime = GetTime();
            spdlog::info("Frametime: {}s", GetFrameTime());
        }

        if (IsKeyPressed(KEY_A))
        {
            cubeRigidbody.mRigidbody->setLinearVelocity({0, 10, 0});
            // cubeRigidbody.mRigidbody->setAngularVelocity({10, 0, 0});
        }

        solver.Solve(world, GetFrameTime());

        BeginDrawing();

        ClearBackground(BLACK);

        renderer.Render(world, camera);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
