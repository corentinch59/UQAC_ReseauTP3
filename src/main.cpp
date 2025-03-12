#include <spdlog/spdlog.h>

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

#include <entt/entt.hpp>

#include <DuoBoloNetwork/Helpers.h>
#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Rendering.h>

int main()
{
    spdlog::info("Hello!");

    InitWindow(1280, 720, "DuoBolo TP3");
    SetTargetFPS(60);
    DisableCursor();

    entt::registry world{};

    Camera camera{};
    camera.position = {0.0f, 4.0f, 10.0f}; // Camera position
    camera.target = {0.0f, 0.0f, 0.0f}; // Camera looking at point
    camera.up = {0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
    camera.fovy = 60.0f; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;
    float cameraMouseSens = 0.003f;
    float cameraSpeed = 5.4f;

    Renderer renderer;
    PhysicsSolver solver;

    Model cube = LoadModelFromMesh(GenMeshCube(1, 1, 1));
    renderer.UpdateMeshMaterialsToUseCorrectShader(cube);
    Model plane = LoadModelFromMesh(GenMeshCube(100, 1, 100));
    renderer.UpdateMeshMaterialsToUseCorrectShader(plane);
    Model sphere = LoadModelFromMesh(GenMeshSphere(.5f, 16, 32));
    renderer.UpdateMeshMaterialsToUseCorrectShader(sphere);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 10; k++)
            {
                auto cubeEntity = world.create();
                auto& cubeTransform = world.emplace_or_replace<Transform>(cubeEntity);
                cubeTransform.scale = {1, 1, 1};
                cubeTransform.translation = Vector3(-5 + j, 2 + k, -5 + i);
                cubeTransform.rotation = QuaternionIdentity();

                auto& cubeRenderable = world.emplace_or_replace<Renderable>(cubeEntity);
                cubeRenderable.model = cube;
                cubeRenderable.tint = Color(
                    GetRandomValue(0, 255),
                    GetRandomValue(0, 255),
                    GetRandomValue(0, 255),
                    255
                );

                Vector3 cubeShapeExtent{1, 1, 1};
                world.emplace_or_replace<Rigidbody>(cubeEntity, 1.f, cubeShapeExtent);
            }
        }
    }

    // ground
    auto planeEntity = world.create();
    auto& planeTransform = world.emplace_or_replace<Transform>(planeEntity);
    planeTransform.scale = {1, 1, 1};
    planeTransform.translation = {0, -.5f, 0};
    planeTransform.rotation = QuaternionIdentity();

    auto& planeRenderable = world.emplace_or_replace<Renderable>(planeEntity);
    planeRenderable.model = plane;
    planeRenderable.tint = GREEN;

    Vector3 planeBoxExtent{100, 1, 100};
    world.emplace_or_replace<Rigidbody>(planeEntity, 0.f, planeBoxExtent);

    float lastPrintTime = GetTime();

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        const Vector2 mouseDelta = GetMouseDelta();
        CameraYaw(&camera, -mouseDelta.x*cameraMouseSens, false);
        CameraPitch(&camera, -mouseDelta.y*cameraMouseSens, true, false, false);
        if (IsKeyDown(KEY_W)) CameraMoveForward(&camera, cameraSpeed*deltaTime, true);
        if (IsKeyDown(KEY_A)) CameraMoveRight(&camera, -cameraSpeed*deltaTime, true);
        if (IsKeyDown(KEY_S)) CameraMoveForward(&camera, -cameraSpeed*deltaTime, true);
        if (IsKeyDown(KEY_D)) CameraMoveRight(&camera, cameraSpeed*deltaTime, true);
        if (IsKeyDown(KEY_Q)) CameraMoveUp(&camera, cameraSpeed*deltaTime);
        if (IsKeyDown(KEY_E)) CameraMoveUp(&camera, -cameraSpeed*deltaTime);

        if (GetTime() - lastPrintTime > 1.0f)
        {
            lastPrintTime = GetTime();
            spdlog::info("Frametime: {}ms", deltaTime * 1000.0f);
        }

        // spawn a sphere
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            auto sphereEntity = world.create();
            auto& sphereTransform = world.emplace_or_replace<Transform>(sphereEntity);
            sphereTransform.scale = {1, 1, 1};
            sphereTransform.translation = camera.position;
            sphereTransform.rotation = QuaternionIdentity();

            auto& sphereRenderable = world.emplace_or_replace<Renderable>(sphereEntity);
            sphereRenderable.model = sphere;
            sphereRenderable.tint = RED;

            auto& sphereRigidbody = world.emplace_or_replace<Rigidbody>(sphereEntity, 1.f, .5f);
            sphereRigidbody.mRigidbody->setLinearVelocity( ToBtVector3(Vector3Normalize(camera.target - camera.position)) * 50.0f);
        }

        solver.Solve(world, deltaTime);

        BeginDrawing();

        ClearBackground(SKYBLUE);

        renderer.Render(world, camera);

        EndDrawing();
    }

    UnloadModel(cube);
    UnloadModel(plane);
    UnloadModel(sphere);

    CloseWindow();

    return 0;
}
