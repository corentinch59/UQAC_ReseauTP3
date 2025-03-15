#pragma once

#include <DuoBoloNetwork/Rendering.h>

#include <entt/entt.hpp>
#include <raylib.h>

class WorldEditor {
  public:
    WorldEditor(entt::registry &world);

    void Update(float dt, Renderer* renderer, Camera* camera);

  private:
    entt::registry &mEnttWorld;
    bool mInCameraMode;

    void CameraMovement(float dt, Camera* camera);
};