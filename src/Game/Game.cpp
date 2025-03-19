#define DBGAME_EXPORT
#include <DuoBoloGame/BaseGame.h>

#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/CameraComponent.h>
#include <spdlog/spdlog.h>

#include <raymath.h>

class DBGAME_API MyGame : public BaseGame {
  public:

    void RegisterComponents(ComponentRegistry& registry) override  {
        spdlog::warn("Registering custom components for MyGame");
    }

    std::string GetStartupSceneName() override 
    { 
        return "assets/Scene1.dbs"; 
    }

    void Init() override {
        spdlog::info("Started MyGame");

        auto entity = GetWorld()->create();

        GetWorld()->emplace<TransformComponent>(entity);

        GetWorld()->emplace<CameraComponent>(entity);
    }

    static inline float time = 0.f;
    void GlobalUpdate(float dt) override {
        time += dt * .5f;
        auto view = GetWorld()->view<TransformComponent, CameraComponent>();

        for (auto && [ entity, transform, camera ] : view.each()) {
            transform.position = {15.f * sinf(time), 8.f, 15.f * cosf(time)};
            transform.rotation = QuaternionFromVector3ToVector3(transform.position, {0.f, 0.f, 0.f});
        }
    }

    void Shutdown() override {
        spdlog::info("Shutting down MyGame");
    }
};

DECLARE_BASE_GAME_CLASS(MyGame)