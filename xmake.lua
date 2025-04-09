add_rules("mode.debug", "mode.release")

-- Shitty fix for shitty bug when using spdlog and raylib on windows
if is_plat("windows") then
    add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN", "NOGDI", "NOUSER", {public = true})
end

add_requires("enet6", "entt", "spdlog", "bullet3", "imgui v1.91.8-docking", "nlohmann_json", "fmt")
add_requires("raylib")

set_project("DuoBoloNetwork")

set_languages("cxx20")
set_exceptions("cxx")
set_encodings("utf-8")
-- set_rundir(".")
add_includedirs("include")
add_installfiles("(assets/**)", {prefixdir = "bin"})

add_cxflags("/wd4251")

if is_mode("rel-editor") then
    add_defines("WITH_SCE_EDITOR")
    set_symbols("hidden")
    set_optimize("fastest")
    set_strip("all")
elseif is_mode("deb-editor") then
    add_defines("WITH_SCE_EDITOR")
    set_symbols("debug")
    set_optimize("none")
elseif is_mode("rel-server") then
    add_defines("IS_SERVER")
    set_symbols("hidden")
    set_optimize("fastest")
    set_strip("all")
elseif is_mode("deb-server") then
    add_defines("IS_SERVER")
    set_symbols("debug")
    set_optimize("none")
end

target("DuoBoloShared")
    set_kind("static")
    add_headerfiles("include/DuoBoloShared/**.hpp","include/DuoBoloShared/**.inl", "include/DuoBoloShared/**.h")
    add_files("src/DuoBoloShared/*.cpp")
    add_packages("entt", "raylib", "spdlog", "nlohmann_json", "imgui", "enet6", {public = true})
    add_deps("rcamera", {public = true})

target("DuoBoloGame")
    set_kind("headeronly")
    add_headerfiles("include/DuoBoloGame/**.hpp","include/DuoBoloGame/**.inl", "include/DuoBoloGame/**.h", {public = true})
    add_packages("entt", "raylib", "spdlog", "fmt", {public = true})
    add_deps("DuoBoloShared", {public = true})

target("Game")
    set_kind("static")
    add_headerfiles("include/Game/**.hpp","include/Game/**.inl", "include/Game/**.h")
    add_files("src/Game/*.cpp")
    add_deps("DuoBoloGame")

target("DuoBoloEngine")
    set_kind("binary")
    add_files("src/DuoboloNetwork/**.cpp")
    add_files("src/main.cpp")
    add_headerfiles("include/DuoBoloNetwork/**.hpp","include/DuoBoloNetwork/**.inl", "include/DuoBoloNetwork/**.h")
    add_packages("imgui", "bullet3", "raylib", "enet6", "spdlog", "entt", "nlohmann_json", "fmt")
    includes("external/rlimgui", "external/rcamera")
    add_deps("rlimgui", "rcamera")
    add_deps("DuoBoloShared")
    add_deps("Game")
    after_build(function (target)
            os.cp("assets", target:targetdir())
    end)