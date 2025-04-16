#pragma once


#include <type_traits>

template<typename T>
std::function<void(entt::handle)> ComponentRegistry::BuildAddComponent()
{
	return [](entt::handle entity)
		{
			return entity.emplace<T>();
		};
}

template<typename T>
std::function<bool(entt::handle)> ComponentRegistry::BuildHasComponent()
{
	return [](entt::handle entity)
		{
			return entity.any_of<T>();
		};
}

template<typename T>
std::function<void(entt::handle)> ComponentRegistry::BuildRemoveComponent()
{
	return [](entt::handle entity)
		{
			return entity.erase<T>();
		};
}

#ifdef WITH_SCE_EDITOR
#include <DuoBoloNetwork/WorldEditor.h>
template <typename T>
auto ComponentRegistry::BuildInspect() -> std::function<void(WorldEditor&, entt::handle)>
{
	return [](WorldEditor& worldEditor, entt::handle entity)
		{
			worldEditor.PopulateInspector<T>(entity);
		};
}
#endif

template<typename T>
std::function<void(entt::handle, std::vector<uint8_t>&)> ComponentRegistry::BuildBinarySerialize()
{
	return [](entt::handle entity, std::vector<uint8_t>& byteArray)
		{
			if constexpr (!std::is_empty_v<T>)
				return entity.get<T>().BinarySerialize(entity, byteArray);
			else
				return;
		};
}

template <typename T>
std::function<void(entt::handle, const std::vector<uint8_t>&, std::size_t&)> ComponentRegistry::BuildBinaryUnserialize()
{
	return [](entt::handle entity, const std::vector<uint8_t>& byteArray, std::size_t& offset)
		{
			if constexpr (!std::is_empty_v<T>)
				return T::BinaryUnserialize(entity, byteArray, offset);
			else
				entity.emplace<T>();
		};
}

template <typename T>
std::function<nlohmann::json(entt::handle)> ComponentRegistry::BuildJsonSerialize()
{
	return [](entt::handle entity)
	{
		if constexpr (!std::is_empty_v<T>)
			return entity.get<T>().JsonSerialize(entity);
		else
			return nlohmann::json::object();
	};
}

template <typename T>
std::function<void(entt::handle, const nlohmann::json&)> ComponentRegistry::BuildJsonUnserialize()
{
	return [](entt::handle entity, const nlohmann::json& doc)
	{
			if constexpr (!std::is_empty_v<T>)
				return T::JsonUnserialize(entity, doc);
			else
				entity.emplace<T>();
	};
}

