﻿#include <DuoBoloNetwork/SceneLoading.h>
#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/ComponentRegistry.h>

#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>

#include <entt/entt.hpp>

#include <fstream>


void LoadSceneFromPath(const std::string& path, entt::registry& world, const ComponentRegistry& componentRegistry)
{
    std::ifstream inputFile(path);
    if (!inputFile)
    {
        spdlog::error("failed to open {}\n", path);
        return;
    }

    nlohmann::json sceneDoc;
    try
    {
        sceneDoc = nlohmann::json::parse(inputFile);
    }
    catch (const std::exception& e)
    {
        spdlog::error("failed to parse {}: {}\n", path, e.what());
        return;
    }

    unsigned int version = sceneDoc["Version"];
    if (version > FILE_VERSION)
    {
        spdlog::error("{} has an unknown file version {}\n", path, version);
        return;
    }

    // On reinitialise le monde pour creer les entites du document
    world.clear(); // try reset()

    std::vector<entt::entity> indexToEntity;
    for (const nlohmann::json& entityDoc : sceneDoc["Entities"])
    {
        // Creation de l'entite
        entt::handle entityHandle(world, world.create());
        indexToEntity.push_back(entityHandle);

        componentRegistry.ForEachComponent([&](const ComponentRegistry::Entry& entry)
            {
                if (!entry.jsonUnserialize)
                    return;

                auto it = entityDoc.find(entry.id);
                if (it != entityDoc.end())
                    entry.jsonUnserialize(entityHandle, it.value());
            });
    }

    for (const nlohmann::json& hierarchyDoc : sceneDoc["Hierarchies"])
    {
        unsigned int parentId = hierarchyDoc["Parent"];
        unsigned int childId = hierarchyDoc["Child"];
        

        //Transform& parentTransform = m_registry.get<Transform>(indexToEntity[parentId]);
        //TransformComponent& childTransform = world.get<TransformComponent>(indexToEntity[childId]);
        //childTransform.
    }

    spdlog::info("successfully loaded {}", path);
}

void SaveSceneToPath(const std::string& path, entt::registry& world, const ComponentRegistry& componentRegistry)
{
    std::ofstream fileStream(path);
    if (!fileStream)
    {
        spdlog::error("failed to open {}", path);
        return;
    }

    struct Hierarchy
    {
        entt::entity parent;
        entt::entity child;
    };

    std::vector<Hierarchy> hierarchies;
    std::unordered_map<entt::entity, unsigned int> entityToIndex;

    // On sauvegarde tous les composants de toutes les entites
    nlohmann::json entityArray;
    for (auto [entity] : world.storage<entt::entity>().each())
    {
        entt::handle entityHandle(world, entity);

        nlohmann::json entityDoc;
        componentRegistry.ForEachComponent([&](const ComponentRegistry::Entry& entry)
            {
                assert(entry.hasComponent);
                if (entry.hasComponent(entityHandle) && entry.jsonSerialize)
                    entityDoc[entry.id] = entry.jsonSerialize(entityHandle);
            });

        if (TransformComponent* transform = entityHandle.try_get<TransformComponent>())
        {
            if (world.valid(transform->parent))
            {
                hierarchies.push_back({ transform->parent, entity });
            }
        }

        entityToIndex[entity] = entityArray.size();
        entityArray.push_back(std::move(entityDoc));
    }

    nlohmann::ordered_json sceneDoc;
    sceneDoc["Version"] = FILE_VERSION;
    sceneDoc["Entities"] = std::move(entityArray);

    fileStream << sceneDoc.dump(1, '\t');

    spdlog::info("scene saved to {}\n", path);
}
