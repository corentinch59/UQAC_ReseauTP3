add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

set_project("DuoBoloNetwork")
set_languages("cxx20")
set_exceptions("cxx")
set_encodings("utf-8")
add_includedirs("include")
add_installfiles("(assets/**)", {prefixdir = "bin"})

-- Dépendances communes
add_requires("enet6", "entt", "spdlog", "bullet3", "imgui v1.91.8-docking", "nlohmann_json", "fmt", "argparse", "raylib")

-- Patch Windows
if is_plat("windows") then
    add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN", "NOGDI", "NOUSER", {public = true})
    add_cxflags("/wd4251")
end

-- Configurations custom
option("with_editor", { description = "Build with editor support", default = false, showmenu = true })
option("is_server", { description = "Build as server", default = false, showmenu = true })

-- Défines globaux liés aux options
if has_config("with_editor") then
    add_defines("WITH_SCE_EDITOR")
end
if has_config("is_server") then
    add_defines("IS_SERVER")
end

-- Target principale partagée
target("Main")
    set_kind("static")
    add_headerfiles("include/(Main/**.hpp)", "include/(Main/**.inl)")
    add_files("src/Main/**.cpp")
    add_packages("spdlog", { public = true })

-- Engine (partagé)
target("DuoBoloEngine")
    set_kind("static")
    add_files("src/DuoBoloNetwork/**.cpp")
    add_headerfiles("include/DuoBoloNetwork/**.hpp","include/DuoBoloNetwork/**.inl", "include/DuoBoloNetwork/**.h")
    add_packages("imgui", "bullet3", "raylib", "enet6", "spdlog", "entt", "nlohmann_json", "fmt", { public = true })
    includes("external/rlimgui", "external/rcamera")
    add_deps("rlimgui", "rcamera")
    after_build(function (target)
        os.cp("assets", target:targetdir())
    end)

-- Fonction utilitaire pour targets clients/serveurs
function duo_target(name, folders, deps, extra_packages)
    target(name)
        set_kind("binary")
        for _, folder in ipairs(folders) do
            add_headerfiles("include/"..folder.."/**.hpp", "include/"..folder.."/**.inl", "include/"..folder.."/**.h")
            add_files("src/"..folder.."/**.cpp")
        end
        add_packages(table.unpack(extra_packages or {}))
        add_deps(table.unpack(deps))
end

duo_target("Editor", {"DuoBoloShared", "DuoBoloClient"}, {"rlimgui", "rcamera", "Main"}, {"raylib", "enet6", "spdlog", "entt", "nlohmann_json", "argparse"})
duo_target("DuoBoloClient", {"DuoBoloShared", "DuoBoloClient"}, {"Main", "DuoBoloEngine", "rlimgui", "rcamera"}, {"raylib", "enet6", "spdlog", "entt", "nlohmann_json", "argparse"})
duo_target("DuoBoloServer", {"DuoBoloShared", "DuoBoloServer"}, {"Main", "DuoBoloEngine"}, {"entt", "enet6", "nlohmann_json", "raylib", "argparse"})
