#include <DuoBoloGame/BaseGame.h>

#include <spdlog/spdlog.h>

class MyGame : public BaseGame
{
public:
  void Init() override
  {
      spdlog::info("Started MyGame");
  }

  void RegisterComponent(ComponentRegistry& registry)
  {

  }

  void GlobalUpdate(float dt) override
  {
      
  }

  void Shutdown() override
  {
      spdlog::info("Shutting down MyGame");
  }
};

DECLARE_BASE_GAME_CLASS(MyGame)