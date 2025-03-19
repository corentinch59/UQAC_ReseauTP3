#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <functional>
#include <vector>
#include <cstdint>

class WorldEditor;

class ComponentRegistry
{
	public:
		struct Entry
		{
			std::string id;
			std::string label;
			std::function<void(entt::handle)> addComponent;
			std::function<bool(entt::handle)> hasComponent;
			std::function<void(entt::handle)> removeComponent;
			std::function<void(WorldEditor&, entt::handle)> inspect;
			std::function<void(entt::handle, std::vector<uint8_t>&)> binarySerialize;
			std::function<void(entt::handle, const std::vector<uint8_t>&)> binaryUnserialize;
			std::function<nlohmann::json(entt::handle)> jsonSerialize;
			std::function<void(entt::handle, const nlohmann::json&)> jsonUnserialize;
		};

		ComponentRegistry();
		ComponentRegistry(const ComponentRegistry&) = delete;
		ComponentRegistry(ComponentRegistry&&) = delete;
		~ComponentRegistry() = default;

		void ForEachComponent(const std::function<void(const Entry&)>& callback) const;

		void Register(Entry&& data);

		ComponentRegistry& operator=(const ComponentRegistry&) = delete;
		ComponentRegistry& operator=(ComponentRegistry&&) = delete;

		template<typename T> static std::function<void(entt::handle)> BuildAddComponent();
		template<typename T> static std::function<bool(entt::handle)> BuildHasComponent();
		template<typename T> static std::function<void(entt::handle)> BuildRemoveComponent();
#ifdef WITH_SCE_EDITOR
		template<typename T> static std::function<void(WorldEditor&, entt::handle)> BuildInspect();
#endif
		template<typename T> static std::function<void(entt::handle, std::vector<uint8_t>&)>  BuildBinarySerialize();
		template<typename T> static std::function<void(entt::handle, const std::vector<uint8_t>&)> BuildBinaryUnserialize();
		template<typename T> static std::function<nlohmann::json(entt::handle)> BuildJsonSerialize();
		template<typename T> static std::function<void(entt::handle, const nlohmann::json&)> BuildJsonUnserialize();

	private:
		void RegisterEngineComponent();
		std::vector<Entry> mComponentTypes;
};

#include <DuoBoloShared/ComponentRegistry.inl>