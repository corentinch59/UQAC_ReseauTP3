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
end

target("DuoBoloShared")
    set_kind("static")
    add_headerfiles("include/DuoBoloShared/**.hpp","include/DuoBoloShared/**.inl", "include/DuoBoloShared/**.h")
    add_files("src/DuoBoloShared/*.cpp")
    add_packages("entt", "raylib", "spdlog", "nlohmann_json", "imgui", {public = true})
    add_deps("rcamera", {public = true})

target("DuoBoloClient")
    set_kind("binary")
    add_files("src/DuoBoloClient/**.cpp")
    add_headerfiles("include/DuoBoloClient/**.hpp","include/DuoBoloClient/**.inl", "include/DuoBoloClient/**.h", {public = true})
    add_packages("entt", "raylib", "spdlog", "fmt", {public = true})
    add_deps("DuoBoloShared", "DuoBoloEngine", {public = true})
    add_defines("DUOBOLO_CLIENT")

target("DuoBoloServer")
    set_kind("binary")
    add_files("src/DuoBoloServer/**.cpp")
    add_headerfiles("include/DuoBoloServer/**.hpp","include/DuoBoloServer/**.inl", "include/DuoBoloServer/**.h", {public = true})
    add_packages("entt", "raylib", "spdlog", "fmt", {public = true})
    add_deps("DuoBoloShared", "DuoBoloEngine", {public = true})
    add_defines("DUOBOLO_SERVER")

target("DuoBoloEngine")
    set_kind("static")
    add_files("src/DuoboloNetwork/**.cpp")
    add_files("src/main.cpp")
    add_headerfiles("include/DuoBoloNetwork/**.hpp","include/DuoBoloNetwork/**.inl", "include/DuoBoloNetwork/**.h")
    add_packages("imgui", "bullet3", "raylib", "enet6", "spdlog", "entt", "nlohmann_json", "fmt")
    includes("external/rlimgui", "external/rcamera")
    add_deps("rlimgui", "rcamera")
    add_deps("DuoBoloShared")
    after_build(function (target)
            os.cp("assets", target:targetdir())
    end)