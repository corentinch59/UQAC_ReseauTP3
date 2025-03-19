
#pragma once

#include <entt/entity/registry.hpp>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/LibraryExport.h>

class DBGAME_API BaseGame {
  public:
    void SetWorld(entt::registry* world)
    {
        mWorld = world;
    }

    virtual ~BaseGame() = default;

    virtual void RegisterComponents(ComponentRegistry& registry) {}

    virtual std::string GetStartupSceneName() { return ""; }

    virtual void Init() {}

    virtual void GlobalUpdate(float dt) {}

    virtual void Shutdown() {}

    entt::registry* GetWorld() const { return mWorld; }

    private:
      entt::registry* mWorld;
};

extern "C" DBGAME_API BaseGame* CreateGameClass();
extern "C" DBGAME_API void DestroyGameClass(BaseGame* game);

#define DECLARE_BASE_GAME_CLASS(c) \
    BaseGame* CreateGameClass() { return new c(); } \
    void DestroyGameClass(BaseGame* game) { delete game; }