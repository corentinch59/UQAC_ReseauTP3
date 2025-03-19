#include <DuoBoloGame/BaseGame.h>

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

	void OnSceneLoaded() override
	{
	}

	void GlobalUpdate(float dt) override
	{
		const Vector2 mouseDelta = GetMouseDelta();
		CameraYaw(&mCamera, -mouseDelta.x * mCameraMouseSens, false);
		CameraPitch(&mCamera, -mouseDelta.y * mCameraMouseSens, true, false, false);
		if (IsKeyDown(KEY_W))
			CameraMoveForward(&mCamera, mCameraSpeed * dt, true);
		if (IsKeyDown(KEY_A))
			CameraMoveRight(&mCamera, -mCameraSpeed * dt, true);
		if (IsKeyDown(KEY_S))
			CameraMoveForward(&mCamera, -mCameraSpeed * dt, true);
		if (IsKeyDown(KEY_D))
			CameraMoveRight(&mCamera, mCameraSpeed * dt, true);
		if (IsKeyDown(KEY_Q))
			CameraMoveUp(&mCamera, mCameraSpeed * dt);
		if (IsKeyDown(KEY_E))
			CameraMoveUp(&mCamera, -mCameraSpeed * dt);

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			auto sphereEntity = GetWorld()->create();
			auto& sphereTransform = GetWorld()->emplace<TransformComponent>(sphereEntity);
			sphereTransform.position = mCamera.position;

			sphereTransform.rotation = QuaternionIdentity();

			auto& sphereRenderable = GetWorld()->emplace<RenderableComponent>(sphereEntity);
			sphereRenderable.model = "sphere";
			sphereRenderable.tint = RED;

			auto& sphereRigidbody = GetWorld()->emplace<RigidbodyComponent>(sphereEntity, 1.f, SphereShape{.5f});
			sphereRigidbody.velocity = Vector3Normalize(mCamera.target - mCamera.position) * 50.0f;
		}
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
};

DECLARE_BASE_GAME_CLASS(MyGame)
