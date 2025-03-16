#include <spdlog/spdlog.h>

#include <raylib.h>
#include <raymath.h>

#include <entt/entt.hpp>

#include <DuoBoloNetwork/Helpers.h>
#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/Transform.h>

#ifdef WITH_SCE_EDITOR
#include <rlImGui.h>
#include <imgui.h>
#include <DuoBoloNetwork/WorldEditor.h>
#endif

#define OBJECT_DESTROY_DISTANCE 1000

int main() {
    spdlog::info("Hello!");

    spdlog::set_level(spdlog::level::debug);

    InitWindow(1280, 720, "DuoBolo TP3");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    // SetTargetFPS(60);
    DisableCursor();

#ifdef WITH_SCE_EDITOR
    rlImGuiSetup(true);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

    entt::registry world{};

#ifndef WITH_SCE_EDITOR
    Camera camera{};
    camera.position = {0.0f, 4.0f, 10.0f}; // Camera position
    camera.target = {0.0f, 0.0f, 0.0f};    // Camera looking at point
    camera.up = {0.0f, 1.0f, 0.0f};        // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                   // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;
#endif

    Renderer renderer;
    PhysicsSolver solver(world);
#ifdef WITH_SCE_EDITOR
    WorldEditor worldEditor(world, &renderer);
#endif

    Mesh cubeMesh = GenMeshCube(1, 1, 1);
    Model cube = LoadModelFromMesh(cubeMesh);
    renderer.UpdateMeshMaterialsToUseCorrectShader(cube);
    Mesh planeMesh = GenMeshCube(100, 1, 100);
    Model plane = LoadModelFromMesh(planeMesh);
    renderer.UpdateMeshMaterialsToUseCorrectShader(plane);
    Mesh sphereMesh = GenMeshSphere(.5f, 16, 32);
    Model sphere = LoadModelFromMesh(sphereMesh);
    renderer.UpdateMeshMaterialsToUseCorrectShader(sphere);

#ifdef WITH_SCE_EDITOR
    renderer.SetRenderIntoTexture(true);
#endif

    for (int i = -5; i <= 5; i++) {
        for (int j = -5; j <= 5; j++) {
            for (int k = 0; k < 2; k++) {
                auto cubeEntity = world.create();
                auto &cubeTransform = world.emplace_or_replace<TransformComponent>(cubeEntity);
                cubeTransform.position = Vector3(j, 2 + k, i);
                cubeTransform.rotation = QuaternionIdentity();

                auto &cubeRenderable = world.emplace_or_replace<RenderableComponent>(cubeEntity);
                cubeRenderable.model = cube;
                cubeRenderable.tint = Color(
                    GetRandomValue(0, 255),
                    GetRandomValue(0, 255),
                    GetRandomValue(0, 255),
                    255);

                world.emplace_or_replace<RigidbodyComponent>(cubeEntity, 1.f, BoxShape{{1, 1, 1}});
            }
        }
    }

    // ground
    auto planeEntity = world.create();
    auto &planeTransform = world.emplace_or_replace<TransformComponent>(planeEntity);
    planeTransform.position = {0, -.5f, 0};
    planeTransform.rotation = QuaternionIdentity();

    auto &planeRenderable = world.emplace_or_replace<RenderableComponent>(planeEntity);
    planeRenderable.model = plane;
    planeRenderable.tint = GREEN;

    world.emplace_or_replace<RigidbodyComponent>(planeEntity, 0.f, BoxShape{{100, 1, 100}});

    float lastPrintTime = GetTime();

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (GetTime() - lastPrintTime > 1.f) {
            lastPrintTime = GetTime();
            spdlog::info("Frametime: {}ms", deltaTime * 1000.0f);
        }

#ifndef WITH_SCE_EDITOR
        // unlock/lock mouse
        if (IsKeyPressed(KEY_F10)) {
            if (IsCursorHidden()) {
                ShowCursor();
                EnableCursor();
            } else {
                HideCursor();
                DisableCursor();
            }
        }
#endif

        // toggle borderless fullscreen
        if (IsKeyPressed(KEY_F11)) {
            ToggleBorderlessWindowed();
        }

        // needed for imgui
        BeginDrawing();
#ifdef WITH_SCE_EDITOR
        rlImGuiBegin();
#endif

#ifndef WITH_SCE_EDITOR
        // destroy objects that are too far
        {
            auto view = world.view<TransformComponent>();

            for (auto && [ entity, transform ] : view.each()) {
                if (Vector3LengthSqr(transform.position) > OBJECT_DESTROY_DISTANCE * OBJECT_DESTROY_DISTANCE)
                    world.destroy(entity);
            }
        }

        solver.Solve(deltaTime);
#endif

        ClearBackground(BLACK);

#ifdef WITH_SCE_EDITOR
        worldEditor.Update(deltaTime);
#else
        renderer.Render(world, camera);
#endif

#ifdef WITH_SCE_EDITOR
        rlImGuiEnd();
        DrawFPS(GetScreenWidth() - 95, 10);
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
