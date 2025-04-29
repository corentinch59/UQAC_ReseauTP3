#pragma once

#include <entt/entity/registry.hpp>
#include <DuoBoloShared/ComponentRegistry.h>
#include <raylib.h>

#include <functional>
#include <DuoBoloNetwork/ClientGameSessionManager.h>
#include <DuoBoloNetwork/Input.h>

class BaseGame
{
public:
	virtual void SetWorld(entt::registry* world)
	{
		mWorld = world;
	}

	virtual ~BaseGame() = default;

	virtual void RegisterComponents(ComponentRegistry* registry)
	{
	}

	virtual std::string GetStartupSceneName() { return ""; }

	virtual void Init()
	{
	}

	virtual void GlobalUpdate(float dt)
	{
	}

	virtual void Shutdown()
	{
	}

	virtual void SetupInput(ClientGameSessionManager& client) {}

	virtual entt::registry* GetWorld() const { return mWorld; }

	virtual void OnSceneLoaded()
	{
	}

	virtual void LoadScene(const std::string& path)
	{
		if (mSceneLoadFunc) mSceneLoadFunc(path);
		OnSceneLoaded();
	}

	virtual void SetLoadSceneFunc(std::function<void(const std::string&)> func)
	{
		mSceneLoadFunc = func;
	}

	virtual PlayerInput GetPlayerInput() { return {}; }

	virtual Camera GetCamera() { return {}; }

	void SetTimeScale(float scale) { mTimeScale = scale;  }

	float GetTimeScale() { return mTimeScale;  }

protected:
	entt::registry* mWorld;
	std::function<void(const std::string&)> mSceneLoadFunc;
	float mTimeScale = 1.0f;
};

BaseGame* CreateGameClass();
void DestroyGameClass(BaseGame* game);

#define DECLARE_BASE_GAME_CLASS(c) \
    BaseGame* CreateGameClass() { return new c(); } \
    void DestroyGameClass(BaseGame* game) { delete game; }
