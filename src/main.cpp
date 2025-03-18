#include <spdlog/spdlog.h>

#include <raylib.h>
#include <raymath.h>

#include <entt/entt.hpp>

#include <DuoBoloNetwork/Helpers.h>
#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/Transform.h>
#include <DuoBoloNetwork/DllLoader.h>

#include <DuoBoloGame/Game.h>
#include <DuoBoloNetwork/ComponentRegistry.h>

#ifdef WITH_SCE_EDITOR
#include <rlImGui.h>
#include <imgui.h>
#include <DuoBoloNetwork/WorldEditor.h>
#endif

#define OBJECT_DESTROY_DISTANCE 1000

void CustomLogCallback(int logLevel, const char *text, va_list args) {
    // Format the log message using the variable arguments
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), text, args);

    // Use spdlog to log the message depending on log level
    switch (logLevel) {
    case LOG_FATAL:
        spdlog::critical("[raylib] {}", buffer); // Critical logs for fatal errors
        break;
    case LOG_ERROR:
        spdlog::error("[raylib] {}", buffer); // Error logs
        break;
    case LOG_WARNING:
        spdlog::warn("[raylib] {}", buffer); // Warning logs
        break;
    case LOG_INFO:
        spdlog::info("[raylib] {}", buffer); // Informational logs
        break;
    case LOG_DEBUG:
        spdlog::debug("[raylib] {}", buffer); // Debug logs
        break;
    default:
        spdlog::info("[raylib] Unknown log level: {}", buffer); // Default case
        break;
    }
}

int main() {
    spdlog::set_level(spdlog::level::debug);

    SetTraceLogCallback(CustomLogCallback);

    InitWindow(1280, 720, "DuoBolo TP3");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    DisableCursor();

    // Game dll loading phase
    DllLoader loader("Game");

    bool isGameDllLoaded = loader.IsDllLoaded();

    if (!isGameDllLoaded)
    {
        spdlog::error("Couldn't load game library");
    } else {
        spdlog::info("Loaded game library");
    }

    using CreateGameFunc = BaseGame *(*)();
    using DestroyGameFunc = void (*)(BaseGame *);

    bool isGameFunctionsLoaded = false;
    CreateGameFunc createGameFunc = nullptr;
    DestroyGameFunc destroyGameFunc = nullptr;
    if (isGameDllLoaded) {
        createGameFunc = loader.LoadFunction<CreateGameFunc>("CreateGameClass");
        destroyGameFunc = loader.LoadFunction<DestroyGameFunc>("DestroyGameClass");

        if (createGameFunc && destroyGameFunc) {
            spdlog::info("Game functions loaded successfully.");
            isGameFunctionsLoaded = true;
        } else {
            spdlog::error("Failed to load game functions!");
        }
    }

    bool gameLoaded = false;
    BaseGame *game = nullptr;
    if (isGameFunctionsLoaded) {
        game = createGameFunc();
        gameLoaded = true;
    }

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
    ComponentRegistry componentRegistry;
#ifdef WITH_SCE_EDITOR
    WorldEditor worldEditor(world, &renderer, componentRegistry);
#endif

#ifdef WITH_SCE_EDITOR
    renderer.SetRenderIntoTexture(true);
#endif

    // init game

    if (gameLoaded)
        game->Init();

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

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

        if (gameLoaded)
            game->GlobalUpdate(deltaTime);

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
#endif
        EndDrawing();
    }

    // clear rigidbodies to avoid errors on bullet deinit
    world.clear<RigidbodyComponent>();

    if (gameLoaded)
        game->Shutdown();


    if (gameLoaded && isGameFunctionsLoaded) {
        destroyGameFunc(game);
    }

#ifdef WITH_SCE_EDITOR
    rlImGuiShutdown();
#endif
    CloseWindow();

    return 0;
}
