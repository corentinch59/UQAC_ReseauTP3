#include <raylib.h>
#include <DuoBoloClient/ClientGame.h>
#include <DuoBoloNetwork/InputManager.h>
#include <rcamera.h>

ClientGame::ClientGame()
{
	mCamera.position = { 0.0f, 4.0f, 10.0f }; // Camera position
	mCamera.target = { 0.0f, 0.0f, 0.0f }; // Camera looking at point
	mCamera.up = { 0.0f, 1.0f, 0.0f }; // Camera up vector (rotation towards target)
	mCamera.fovy = 60.0f; // Camera field-of-view Y
	mCamera.projection = CAMERA_PERSPECTIVE;
}

void ClientGame::SetupInput()
{
	InputManager::Instance().BindKey(KEY_W, "forward");
	InputManager::Instance().BindKey(KEY_A, "left");
	InputManager::Instance().BindKey(KEY_S, "backward");
	InputManager::Instance().BindKey(KEY_D, "right");
	InputManager::Instance().BindKey(KEY_Q, "up");
	InputManager::Instance().BindKey(KEY_E, "down");
	InputManager::Instance().BindMouse(MOUSE_BUTTON_LEFT, "shoot");
	//InputManager::Instance().BindKey(KEY_F10, "showCursor");
}

void ClientGame::UpdateCameraRotation()
{
	const Vector2 mouseDelta = GetMouseDelta();
	const float movementMult = IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f;
	CameraYaw(&mCamera, -mouseDelta.x * mCameraMouseSens, false);
	CameraPitch(&mCamera, -mouseDelta.y * mCameraMouseSens, true, false, false);
}
