#ifdef WITH_SCE_EDITOR

#include <DuoBoloNetwork/WorldEditor.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/RenderableComponent.h>
#include <DuoBoloShared/PhysicsComponent.h>
#include <DuoBoloShared/NameComponent.h>
#include <DuoBoloNetwork/SceneLoading.h>

#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <rlgl.h>
#include <raymath.h>
#include <rcamera.h>

#include <spdlog/spdlog.h>
#include <fmt/format.h>

#define ImGuiWindowFlags_NoInputIfCamera (mInCameraMode ? ImGuiWindowFlags_NoInputs : ImGuiWindowFlags_None)

WorldEditor::WorldEditor(entt::registry& world, Renderer& renderer, WorldSettings& wSettings,
                         const ComponentRegistry& componentRegistry, std::shared_ptr<ImGuiSpdlogSinkMt> logSink) :
	mEnttWorld(world),
	mRenderer(&renderer),
	mWSettings(&wSettings),
	mComponentRegistry(componentRegistry),
	mSink(logSink)
{
	mInCameraMode = false;

	mCamera = {};
	mCamera.position = {0.0f, 4.0f, 10.0f}; // Camera position
	mCamera.target = {0.0f, 0.0f, 0.0f}; // Camera looking at point
	mCamera.up = {0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
	mCamera.fovy = 60.0f; // Camera field-of-view Y
	mCamera.projection = CAMERA_PERSPECTIVE;

	mDockingSpaceCreated = false;

	Image cubeImg = LoadImage("assets/cube_preview.jpg");
	mCubeImage = LoadTextureFromImage(cubeImg);
	UnloadImage(cubeImg);
}

void WorldEditor::Update(float dt)
{
	if (IsKeyPressed(KEY_F10))
	{
		mInCameraMode = !mInCameraMode;

		if (mInCameraMode)
		{
			HideCursor();
			DisableCursor();
		}
		else
		{
			ShowCursor();
			EnableCursor();
		}
	}


	FullscreenDockingSpace();

	ViewportWindow();

	HierarchyWindow();

	InspectorWindow();

	ShapeBrowserWindow();

	if (mInCameraMode)
	{
		CameraMovement(dt, &mCamera);
	}

	mRenderer->Render(mEnttWorld, mCamera, *mWSettings);

	if (mSelected != entt::null)
	{
		// render selected icon
		if (mEnttWorld.any_of<TransformComponent>(mSelected))
		{
			auto& transform = mEnttWorld.get<TransformComponent>(mSelected);
			if (mEnttWorld.any_of<RenderableComponent>(mSelected))
			{
				auto& renderable = mEnttWorld.get<RenderableComponent>(mSelected);

				Vector3 axis;
				float angle;
				QuaternionToAxisAngle(transform.rotation, &axis, &angle);

				BeginTextureMode(mRenderer->GetRenderTexture());
				BeginMode3D(mCamera);
				rlDisableDepthTest();
				DrawModelWiresEx(mRenderer->GetModel(renderable.model), transform.position, axis, angle * RAD2DEG,
				                 transform.scale, RED);
				rlEnableDepthTest();
				EndMode3D();
				EndTextureMode();
			}

			Vector2 positionOnScreen = GetWorldToScreenEx(transform.position, mCamera, mRenderer->GetWidth(),
			                                              mRenderer->GetHeight());

			BeginTextureMode(mRenderer->GetRenderTexture());
			DrawCircle(positionOnScreen.x, positionOnScreen.y, 10.f, BLACK);
			DrawCircle(positionOnScreen.x, positionOnScreen.y, 9.f, WHITE);
			EndTextureMode();
		}
	}

	mSink->DrawConsole(dt, gConsoleWindowName, ImGuiWindowFlags_NoInputIfCamera);
}

void WorldEditor::CameraMovement(float deltaTime, Camera* camera)
{
	float cameraMouseSens = 0.003f;
	float cameraSpeed = 5.4f;

	const Vector2 mouseDelta = GetMouseDelta();
	CameraYaw(camera, -mouseDelta.x * cameraMouseSens, false);
	CameraPitch(camera, -mouseDelta.y * cameraMouseSens, true, false, false);
	if (IsKeyDown(KEY_W))
		CameraMoveForward(camera, cameraSpeed * deltaTime, true);
	if (IsKeyDown(KEY_A))
		CameraMoveRight(camera, -cameraSpeed * deltaTime, true);
	if (IsKeyDown(KEY_S))
		CameraMoveForward(camera, -cameraSpeed * deltaTime, true);
	if (IsKeyDown(KEY_D))
		CameraMoveRight(camera, cameraSpeed * deltaTime, true);
	if (IsKeyDown(KEY_Q))
		CameraMoveUp(camera, cameraSpeed * deltaTime);
	if (IsKeyDown(KEY_E))
		CameraMoveUp(camera, -cameraSpeed * deltaTime);
}

void WorldEditor::FullscreenDockingSpace()
{
	auto viewport = ImGui::GetMainViewport();

	// Submit a window filling the entire viewport
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags hostWindowFlags = 0;
	hostWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	hostWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	hostWindowFlags |= ImGuiWindowFlags_MenuBar;

	char label[32];
	ImFormatString(label, IM_ARRAYSIZE(label), "WindowOverViewport_%08X", viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(label, nullptr, hostWindowFlags);
	ImGui::PopStyleVar(3);

	MainMenuBar();

	// Submit the dockspace
	ImGuiID dockspaceID = ImGui::GetID("DockSpace");

	if (!mDockingSpaceCreated)
	{
		mDockingSpaceCreated = true;

		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

		ImGuiID bottomID;
		ImGuiID topID = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Up, 0.8f, nullptr, &bottomID);
		ImGuiID topLeftID;
		ImGuiID topRightID = ImGui::DockBuilderSplitNode(topID, ImGuiDir_Right, 0.8f, nullptr, &topLeftID);
		ImGuiID topMiddleID;
		topRightID = ImGui::DockBuilderSplitNode(topRightID, ImGuiDir_Right, 0.2f, nullptr, &topMiddleID);

		ImGui::DockBuilderDockWindow(gInspectorWindowName, topRightID);
		ImGui::DockBuilderDockWindow(gViewportWindowName, topMiddleID);
		ImGui::DockBuilderDockWindow(gHierarchyWindowName, topLeftID);
		ImGui::DockBuilderDockWindow(gConsoleWindowName, bottomID);
		ImGui::DockBuilderDockWindow(gShapeBrowserWindowName, bottomID);

		ImGui::DockBuilderFinish(dockspaceID);
	}

	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::End();
}

void WorldEditor::MainMenuBar()
{
	// menu bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::Text(fmt::format("Loaded Scene : {}", mScenePath).c_str());

			ImGui::Text("Path");

			ImGui::SameLine();

			ImGui::InputText("##pathInput", &mScenePath);
			if (ImGui::Button("Save scene"))
				SaveScene();

			ImGui::SameLine();

			if (ImGui::Button("Load scene"))
				LoadScene();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void WorldEditor::ViewportWindow()
{
	ImGui::Begin(gViewportWindowName, nullptr, ImGuiWindowFlags_NoInputIfCamera | ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::Checkbox("Auto adapt renderer size", &mAutoAdapt);
			if (mAutoAdapt)
				ImGui::BeginDisabled();
			ImGui::InputInt("Width", &mNewWidth, 1);
			ImGui::InputInt("Height", &mNewHeight, 1);
			if (ImGui::Button("Apply"))
			{
				mRenderer->SetRenderSize(mNewWidth, mNewHeight);
			}
			if (mAutoAdapt)
				ImGui::EndDisabled();
			ImGui::EndMenu();
		}
		ImGui::Text("Framerate: %i FPS", GetFPS());
		ImGui::EndMenuBar();
	}

	if (mAutoAdapt)
	{
		auto size = ImGui::GetContentRegionAvail();
		mNewWidth = size.x;
		mNewHeight = size.y;
		mRenderer->SetRenderSize(mNewWidth, mNewHeight);
	}

	RenderTexture2D rt = mRenderer->GetRenderTexture();
	// rlImGuiImageRenderTexture(&rt);

	ImVec2 avail = ImGui::GetContentRegionAvail(); // Available space
	float imgAspect = (float)mRenderer->GetWidth() / (float)mRenderer->GetHeight();
	float availAspect = avail.x / avail.y;

	ImVec2 imageSize;
	if (imgAspect > availAspect)
	{
		// Image is wider than available space
		imageSize.x = avail.x;
		imageSize.y = avail.x / imgAspect;
	}
	else
	{
		// Image is taller than available space
		imageSize.y = avail.y;
		imageSize.x = avail.y * imgAspect;
	}

	ImVec2 cursorPos = ImGui::GetCursorScreenPos(); // Get current cursor position
	ImVec2 centerPos = ImVec2(
		cursorPos.x + (avail.x - imageSize.x) * 0.5f,
		cursorPos.y + (avail.y - imageSize.y) * 0.5f
	);

	ImGui::SetCursorScreenPos(centerPos);
	ImGui::Image((ImTextureID)mRenderer->GetRenderTexture().texture.id, imageSize, ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
	{
		mInCameraMode = true;
		HideCursor();
		DisableCursor();
	}
	else if (mInCameraMode && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
	{
		mInCameraMode = false;
		ShowCursor();
		EnableCursor();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_CUBE"))
		{
			int dragId = 0;
			std::memcpy(&dragId, payload->Data, payload->DataSize);
			if (dragId == 1)
			{
				entt::handle entity(mEnttWorld, mEnttWorld.create());
				entity.emplace<NameComponent>("Cube");
				entity.emplace<TransformComponent>();
				entity.emplace<RenderableComponent>();
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}

void WorldEditor::HierarchyWindow()
{
	ImGui::Begin(gHierarchyWindowName, nullptr, ImGuiWindowFlags_NoInputIfCamera);


	for (auto entity : mEnttWorld.view<entt::entity>())
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (mSelected == entity)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		std::string entityName = fmt::format("Entity #{}", static_cast<std::uint32_t>(entity));
		if (NameComponent* nameComponent = mEnttWorld.try_get<NameComponent>(entity))
		{
			if (!nameComponent->name.empty())
				entityName = fmt::format("{} - {}", entityName, nameComponent->name);
		}

		bool open = ImGui::TreeNodeEx(entityName.c_str(), flags);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			spdlog::info("Selected entity {}", (uint32_t)entity);
			mSelected = entity;
		}

		if (open)
		{
			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void WorldEditor::InspectorWindow()
{
	ImGui::Begin(gInspectorWindowName, nullptr, ImGuiWindowFlags_NoInputIfCamera);

	if (mSelected != entt::null)
	{
		entt::handle entityHandle(mEnttWorld, mSelected);
		bool canAddComponent = false;
		mComponentRegistry.ForEachComponent([&](const ComponentRegistry::Entry& entry)
		{
			assert(entry.hasComponent);
			if (entry.hasComponent(entityHandle))
			{
				if (!ImGui::TreeNode(entry.label.c_str()))
					return;

				if (entry.inspect)
					entry.inspect(*this, entityHandle);

				if (entry.removeComponent && ImGui::Button(fmt::format("Remove {} component", entry.label).c_str()))
					entry.removeComponent(entityHandle);

				ImGui::TreePop();
			}
			else if (entry.addComponent)
				canAddComponent = true;
		});

		if (canAddComponent)
		{
			if (ImGui::Button("Add component..."))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				bool closePopup = false;
				if (ImGui::BeginCombo("Add component", "Choose..."))
				{
					mComponentRegistry.ForEachComponent([&](const ComponentRegistry::Entry& entry)
					{
						assert(entry.hasComponent);
						if (entry.hasComponent(entityHandle))
							return;

						if (entry.addComponent && ImGui::Selectable(entry.label.c_str()))
						{
							entry.addComponent(entityHandle);
							closePopup = true;
						}
					});

					ImGui::EndCombo();
				}

				if (closePopup)
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
		}

		std::string deleteModalId = fmt::format("Delete {}", static_cast<std::uint32_t>(mSelected));
		if (ImGui::Button("Delete entity"))
			ImGui::OpenPopup(deleteModalId.c_str());

		if (ImGui::BeginPopupModal(deleteModalId.c_str(), nullptr,
		                           ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
		{
			ImGui::Text("Are you sure you want to delete this entity?\nThis operation cannot be undone!\n\n");
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				mEnttWorld.destroy(mSelected);
				mSelected = entt::null;;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}

	ImGui::End();
}

void WorldEditor::ShapeBrowserWindow()
{
	ImGui::Begin(gShapeBrowserWindowName, nullptr, ImGuiWindowFlags_NoInputIfCamera);

	ImGui::ImageButton("Cube", mCubeImage.id, ImVec2(100.f, 100.f));
	if (ImGui::BeginDragDropSource())
	{
		const char* cubeId = new char(1);
		ImGui::SetDragDropPayload("ENTITY_CUBE", cubeId, sizeof(char));
		delete(cubeId);

		ImGui::EndDragDropSource();
	}
	ImGui::TextWrapped("Cube");

	ImGui::End();
}

void WorldEditor::LoadScene()
{
	LoadSceneFromPath(mScenePath, mEnttWorld, mComponentRegistry);
}

void WorldEditor::SaveScene()
{
	SaveSceneToPath(mScenePath, mEnttWorld, mComponentRegistry);
}

// Inspector

template <>
void WorldEditor::PopulateInspector<TransformComponent>(entt::handle entity)
{
	ImGui::Text("Position");

	ImGui::SameLine();

	auto& t = entity.get<TransformComponent>();
	float posArray[3] = {t.position.x, t.position.y, t.position.z};
	if (ImGui::DragFloat3("##inputPosition", posArray, .05f))
	{
		t.position.x = posArray[0];
		t.position.y = posArray[1];
		t.position.z = posArray[2];
	}

	ImGui::Text("Rotation");

	ImGui::SameLine();
	Vector3 euler = QuaternionToEuler(t.rotation) * RAD2DEG;
	float rotArray[3] = {euler.x, euler.y, euler.z};
	if (ImGui::DragFloat3("##inputRotation", rotArray, .25f))
	{
		t.rotation = QuaternionFromEuler(rotArray[0] * DEG2RAD, rotArray[1] * DEG2RAD, rotArray[2] * DEG2RAD);
	}

	ImGui::Text("Scale");

	ImGui::SameLine();

	float scaleArray[3] = {t.scale.x, t.scale.y, t.scale.z};
	if (ImGui::DragFloat3("##inputScale", scaleArray, .05f))
	{
		t.scale.x = scaleArray[0];
		t.scale.y = scaleArray[1];
		t.scale.z = scaleArray[2];
	}
}

template <>
void WorldEditor::PopulateInspector<RenderableComponent>(entt::handle entity)
{
	auto& r = entity.get<RenderableComponent>();

	ImGui::Text("Model");

	ImGui::SameLine();

	ImGui::InputText("##modelInput", &r.model);

	ImGui::Text("Tint");

	ImGui::SameLine();
	float color[4]{(float)r.tint.r / 255.f, (float)r.tint.g / 255.f, (float)r.tint.b / 255.f, (float)r.tint.a / 255.f};
	if (ImGui::ColorEdit4("##tintInput", color))
	{
		r.tint.r = static_cast<unsigned char>(color[0] * 255.f);
		r.tint.g = static_cast<unsigned char>(color[1] * 255.f);
		r.tint.b = static_cast<unsigned char>(color[2] * 255.f);
		r.tint.a = static_cast<unsigned char>(color[3] * 255.f);
	}
}

template <>
void WorldEditor::PopulateInspector<RigidbodyComponent>(entt::handle entity)
{
	auto& r = entity.get<RigidbodyComponent>();

	ImGui::Text("Mass");

	ImGui::SameLine();

	ImGui::DragFloat("##inputMass", &r.mass);

	ImGui::Text("RigidBody Shape");

	ImGui::SameLine();

	int id;
	if (r.shape.type().index() == entt::type_index<BoxShape>())
		id = 0;
	else
		id = 1;

	const char* shapes[]{"Box", "Sphere"};
	if (ImGui::Combo("##inputRigidBodyshape", &id, shapes, IM_ARRAYSIZE(shapes)))
	{
	}
}

template <>
void WorldEditor::PopulateInspector<NameComponent>(entt::handle entity)
{
	auto& n = entity.get<NameComponent>();
	ImGui::InputText("Name", &n.name);
}


#endif
