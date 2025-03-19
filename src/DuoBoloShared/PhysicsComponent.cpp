#include <DuoBoloShared/PhysicsComponent.h>
#include <DuoBoloShared/BinarySerializer.h>
#include <DuoBoloShared/JsonSerializer.h>

#include <nlohmann/json.hpp>

void RigidbodyComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{
}

void RigidbodyComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray)
{
}

nlohmann::json RigidbodyComponent::JsonSerialize(const entt::handle entity) const
{
	nlohmann::json doc;

	doc["Mass"] = mass;
	if(shape.type().index() == entt::type_index<BoxShape>())
	{
		auto boxShape = entt::any_cast<BoxShape>(shape);
		doc["Shape"] = "Box";
		doc["Box"] = boxShape;
	}
	else
	{
		auto sphereShape = entt::any_cast<SphereShape>(shape);
		doc["Shape"] = "Sphere";
		doc["Sphere"] = sphereShape;
	}

	return doc;
}

void RigidbodyComponent::JsonUnserialize(entt::handle entity, const nlohmann::json& doc)
{
	std::string shapestr = doc.value("Shape", "Box");
	entt::any shape;
	if(shapestr == "Box")
	{
		shape = BoxShape(doc.value("Box", BoxShape{1.f,1.f,1.f}));
	}
	else
	{
		shape = SphereShape(doc.value("Sphere", SphereShape{ 1.f }));
	}

	float mass = doc.value("Mass", 1.f);
	entity.emplace<RigidbodyComponent>(mass, shape);
}
