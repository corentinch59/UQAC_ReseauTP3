#include <DuoBoloGame/Game.h>

#include <spdlog/spdlog.h>

class MyGame : public BaseGame
{
public:
  void Init() override
  {
      spdlog::info("Started MyGame");
  }

  void GlobalUpdate(entt::registry& world, float dt) override
  {
      
  }

  void Shutdown() override
  {
      spdlog::info("Shutting down MyGame");
  }
};

DECLARE_BASE_GAME_CLASS(MyGame)