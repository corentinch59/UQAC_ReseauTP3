#include <spdlog/spdlog.h>

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

#include <entt/entt.hpp>

#include <DuoBoloNetwork/Helpers.h>
#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/Transform.h>

#ifdef WITH_SCE_EDITOR
#include <imgui.h>
#include <rlImGui.h>
#endif

#define OBJECT_DESTROY_DISTANCE 1000

int main()
{
    spdlog::info("Hello!");

    spdlog::set_level(spdlog::level::debug);

    InitWindow(1280, 720, "DuoBolo TP3");
    SetTargetFPS(60);
    DisableCursor();
#ifdef WITH_SCE_EDITOR
    rlImGuiSetup(true);
    ImGui::GetIO().MouseDrawCursor = false;
#endif


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
    PhysicsSolver solver(world);

    Mesh cubeMesh = GenMeshCube(1, 1, 1);
    Model cube = LoadModelFromMesh(cubeMesh);
    renderer.UpdateMeshMaterialsToUseCorrectShader(cube);
    Mesh planeMesh = GenMeshCube(100, 1, 100);
    Model plane = LoadModelFromMesh(planeMesh);
    renderer.UpdateMeshMaterialsToUseCorrectShader(plane);
    Mesh sphereMesh = GenMeshSphere(.5f, 16, 32);
    Model sphere = LoadModelFromMesh(sphereMesh);
    renderer.UpdateMeshMaterialsToUseCorrectShader(sphere);

    for (int i = -5; i <= 5; i++)
    {
        for (int j = -5; j <= 5; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                auto cubeEntity = world.create();
                auto& cubeTransform = world.emplace_or_replace<TransformComponent>(cubeEntity);
                cubeTransform.position = Vector3(j, 2 + k, i);
                cubeTransform.rotation = QuaternionIdentity();

                auto& cubeRenderable = world.emplace_or_replace<RenderableComponent>(cubeEntity);
                cubeRenderable.model = cube;
                cubeRenderable.tint = Color(
                    GetRandomValue(0, 255),
                    GetRandomValue(0, 255),
                    GetRandomValue(0, 255),
                    255
                );

                world.emplace_or_replace<RigidbodyComponent>(cubeEntity, 1.f, BoxShape{{1, 1, 1}});
            }
        }
    }

    // ground
    auto planeEntity = world.create();
    auto& planeTransform = world.emplace_or_replace<TransformComponent>(planeEntity);
    planeTransform.position = {0, -.5f, 0};
    planeTransform.rotation = QuaternionIdentity();

    auto& planeRenderable = world.emplace_or_replace<RenderableComponent>(planeEntity);
    planeRenderable.model = plane;
    planeRenderable.tint = GREEN;

    world.emplace_or_replace<RigidbodyComponent>(planeEntity, 0.f, BoxShape{{100, 1, 100}});

    float lastPrintTime = GetTime();

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // needed for imgui
        BeginDrawing();
#ifdef WITH_SCE_EDITOR
        rlImGuiBegin();
        ImGui::ShowDemoWindow();
#endif

        const Vector2 mouseDelta = GetMouseDelta();
        CameraYaw(&camera, -mouseDelta.x * cameraMouseSens, false);
        CameraPitch(&camera, -mouseDelta.y * cameraMouseSens, true, false, false);
        if (IsKeyDown(KEY_W)) CameraMoveForward(&camera, cameraSpeed * deltaTime, true);
        if (IsKeyDown(KEY_A)) CameraMoveRight(&camera, -cameraSpeed * deltaTime, true);
        if (IsKeyDown(KEY_S)) CameraMoveForward(&camera, -cameraSpeed * deltaTime, true);
        if (IsKeyDown(KEY_D)) CameraMoveRight(&camera, cameraSpeed * deltaTime, true);
        if (IsKeyDown(KEY_Q)) CameraMoveUp(&camera, cameraSpeed * deltaTime);
        if (IsKeyDown(KEY_E)) CameraMoveUp(&camera, -cameraSpeed * deltaTime);

        if (GetTime() - lastPrintTime > 1.0f)
        {
            lastPrintTime = GetTime();
            spdlog::info("Frametime: {}ms", deltaTime * 1000.0f);
        }

        // destroy objects that are too far
        {
            auto view = world.view<TransformComponent>();

            for (auto&& [entity, transform] : view.each())
            {
                if (Vector3LengthSqr(transform.position) > OBJECT_DESTROY_DISTANCE * OBJECT_DESTROY_DISTANCE) world.
                    destroy(entity);
            }
        }

        // spawn a sphere
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            auto sphereEntity = world.create();
            auto& sphereTransform = world.emplace_or_replace<TransformComponent>(sphereEntity);
            sphereTransform.position = camera.position;
            sphereTransform.rotation = QuaternionIdentity();

            auto& sphereRenderable = world.emplace_or_replace<RenderableComponent>(sphereEntity);
            sphereRenderable.model = sphere;
            sphereRenderable.tint = RED;

            auto& sphereRigidbody = world.emplace_or_replace<RigidbodyComponent>(sphereEntity, 10.f, SphereShape{.5f});
            sphereRigidbody.velocity = Vector3Normalize(camera.target - camera.position) * 50.0f;
        }

        solver.Solve(deltaTime);

        ClearBackground(SKYBLUE);

        renderer.Render(world, camera);

#ifdef WITH_SCE_EDITOR
        rlImGuiEnd();
#endif
        EndDrawing();
    }

    // clear rigidbodies to avoid errors on bullet deinit
    world.clear<RigidbodyComponent>();

    UnloadModel(cube);
    UnloadModel(plane);
    UnloadModel(sphere);

#ifdef WITH_SCE_EDITOR
    rlImGuiShutdown();
#endif
    CloseWindow();

    return 0;
}
