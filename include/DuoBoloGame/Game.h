
#pragma once

#include <entt/entity/registry.hpp>

#ifdef _WIN32
#define DBGAME_API __declspec(dllexport)
#else
#define DBGAME_API __attribute__((visibility("default")))
#endif

class DBGAME_API BaseGame {
public:
    virtual ~BaseGame() = default;

    virtual void Init() {}

    virtual void GlobalUpdate(entt::registry & world, float dt) {}

    virtual void Shutdown() {}
};

#define DECLARE_BASE_GAME_CLASS(c) \
    extern "C" DBGAME_API BaseGame* CreateGameClass() { return new c(); } \
    extern "C" DBGAME_API void DestroyGameClass(BaseGame* game) { delete game; }