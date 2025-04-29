#include <DuoBoloGame/BaseGame.h>

#include <DuoBoloNetwork/InputManager.h>
#include <DuoBoloNetwork/Input.h>

#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/RenderableComponent.h>
#include <DuoBoloShared/PhysicsComponent.h>
#include <DuoBoloShared/Maths.h>
#include <spdlog/spdlog.h>

#include <raymath.h>
#include <rcamera.h>


class MyGame : public BaseGame
{
public:
	void RegisterComponents(ComponentRegistry* registry) override
	{
		spdlog::warn("Registering custom components for MyGame");
	}

	std::string GetStartupSceneName() override
	{
		return "assets/Scene1.dbs";
	}

	float mFireRate = 20.f;
	float lastShootTime = 0;

	void Init() override
	{
		spdlog::info("Started MyGame");

		mCamera = {};
		mCamera.position = {0.0f, 4.0f, 10.0f}; // Camera position
		mCamera.target = {0.0f, 0.0f, 0.0f}; // Camera looking at point
		mCamera.up = {0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
		mCamera.fovy = 60.0f; // Camera field-of-view Y
		mCamera.projection = CAMERA_PERSPECTIVE;
	}

	void SetupInput(ClientGameSessionManager& client) override
	{
		InputManager::Instance().BindKeyboardAction("forward",
			[&](InputAction a)
			{
				CameraMoveForward(&mCamera, mCameraSpeed * a.deltaTime * mMovementMult, true);
				if(a.state == PressState::Press)
				{
					playerInput.forward = true;
				}

				if(a.state == PressState::Release)
				{
					playerInput.forward = false;
				}

				client.SendInputPacket(playerInput);
			});
		InputManager::Instance().BindKeyboardAction("left",
			[&](InputAction a)
			{
				CameraMoveRight(&mCamera, -mCameraSpeed * a.deltaTime * mMovementMult, true);
				if(a.state == PressState::Press)
				{
					playerInput.left = true;
				}

				if(a.state == PressState::Down)
				{
					playerInput.left = false;
				}

				client.SendInputPacket(playerInput);
			});
		InputManager::Instance().BindKeyboardAction("backward",
			[&](InputAction a)
			{
				CameraMoveForward(&mCamera, -mCameraSpeed * a.deltaTime * mMovementMult, true);
				if(a.state == PressState::Press)
				{
					playerInput.backward = true;
				}

				if(a.state == PressState::Down)
				{
					playerInput.backward = false;
				}

				client.SendInputPacket(playerInput);
			});
		InputManager::Instance().BindKeyboardAction("right",
			[&](InputAction a)
			{
				CameraMoveRight(&mCamera, mCameraSpeed * a.deltaTime * mMovementMult, true);
				if(a.state == PressState::Press)
				{
					playerInput.right = true;
				}

				if(a.state == PressState::Down)
				{
					playerInput.right = false;
				}

				client.SendInputPacket(playerInput);
			});
		InputManager::Instance().BindKeyboardAction("up",
			[&](InputAction a)
			{
				CameraMoveUp(&mCamera, mCameraSpeed * a.deltaTime * mMovementMult);
				if(a.state == PressState::Press)
				{
					playerInput.up = true;
				}

				if(a.state == PressState::Down)
				{
					playerInput.up = false;
				}

				client.SendInputPacket(playerInput);
			});
		InputManager::Instance().BindKeyboardAction("down",
			[&](InputAction a)
			{
				CameraMoveUp(&mCamera, -mCameraSpeed * a.deltaTime * mMovementMult);
				if(a.state == PressState::Press)
				{
					playerInput.down = true;
				}

				if(a.state == PressState::Down)
				{
					playerInput.down = false;
				}

				client.SendInputPacket(playerInput);
			});
		InputManager::Instance().BindKeyboardAction("showCursor",
			[](InputAction a)
			{
				if (a.state != PressState::Press)
					return;

				if (IsCursorHidden())
				{
					ShowCursor();
					EnableCursor();
				}
				else
				{
					HideCursor();
					DisableCursor();
				}
			});
		InputManager::Instance().BindMouseAction("Shoot",
			[&](InputAction a)
			{
				if(a.state == PressState::Press)
				{
					playerInput.shoot = true;
				}

				if(a.state == PressState::Down)
				{
					playerInput.shoot = false;
				}

				client.SendShootPacket(mCamera.position, mCamera.target - mCamera.position);
			});

		InputManager::Instance().BindKey(KEY_W, "forward");
		InputManager::Instance().BindKey(KEY_A, "left");
		InputManager::Instance().BindKey(KEY_S, "backward");
		InputManager::Instance().BindKey(KEY_D, "right");
		InputManager::Instance().BindKey(KEY_Q, "up");
		InputManager::Instance().BindKey(KEY_E, "down");
		InputManager::Instance().BindMouse(MOUSE_BUTTON_LEFT, "shoot");
		InputManager::Instance().BindKey(KEY_F10, "showCursor");
	}

	void OnSceneLoaded() override
	{
	}

	void GlobalUpdate(float dt) override
	{
		const Vector2 mouseDelta = GetMouseDelta();
		const float movementMult = IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f;
		CameraYaw(&mCamera, -mouseDelta.x * mCameraMouseSens, false);
		CameraPitch(&mCamera, -mouseDelta.y * mCameraMouseSens, true, false, false);
	}

	void Shutdown() override
	{
		spdlog::info("Shutting down MyGame");
	}

	Camera GetCamera() override
	{
		return mCamera;
	}

private:
	Camera mCamera{};
	float mCameraMouseSens = 0.003f;
	float mCameraSpeed = 5.4f;
	float mMovementMult = 1.f;
	PlayerInput playerInput;
};

DECLARE_BASE_GAME_CLASS(MyGame)