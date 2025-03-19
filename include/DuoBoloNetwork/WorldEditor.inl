#include "DuoBoloShared/TransformComponent.h"
#include <spdlog/spdlog.h>

struct TransformComponent;

template <typename T>
void WorldEditor::PopulateInspector(const entt::handle& entity)
{
	spdlog::error("Type inspector not implemented");
}

template<>
void WorldEditor::PopulateInspector<TransformComponent>(const entt::handle& entity)
{
	auto& t = entity.get<TransformComponent>();
	float posArray[3] = { t.position.x, t.position.y, t.position.z };
	if (ImGui::InputFloat3("Position", posArray))
	{
		t.position.x = posArray[0];
		t.position.y = posArray[1];
		t.position.z = posArray[2];
	}

	float rotArray[4] = { t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w };
	if (ImGui::InputFloat3("Rotation Quaternion", rotArray))
	{
		t.rotation.x = rotArray[0];
		t.rotation.y = rotArray[1];
		t.rotation.z = rotArray[2];
		t.rotation.w = rotArray[3];
	}

	float scaleArray[3] = { t.scale.x, t.scale.y, t.scale.z };
	if (ImGui::InputFloat3("Scale", scaleArray))
	{
		t.scale.x = scaleArray[0];
		t.scale.y = scaleArray[1];
		t.scale.z = scaleArray[2];
	}
}