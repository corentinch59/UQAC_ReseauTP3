#pragma once

#include <raylib.h>

#include <DuoBoloNetwork/Input.h>

class ClientGame
{
public:
	ClientGame();

	void SetupInput();

	void UpdateCameraRotation();

	Camera& GetCamera() { return mCamera; }
	float GetCameraSpeed() const { return mCameraSpeed; }
	PlayerInput& GetPlayerInput() { return playerInput; }

private:
	Camera mCamera;
	float mCameraMouseSens = 0.003f;
	float mCameraSpeed = 5.4f;
	PlayerInput playerInput;
};


