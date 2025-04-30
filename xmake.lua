add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")


-- Shitty fix for shitty bug when using spdlog and raylib on windows
if is_plat("windows") then
    add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN", "NOGDI", "NOUSER", {public = true})
end

add_requires("enet6", "entt", "spdlog", "bullet3", "imgui v1.91.8-docking", "nlohmann_json", "fmt", "argparse")
add_requires("raylib")

set_project("DuoBoloNetwork")

set_languages("cxx20")
set_exceptions("cxx")
set_encodings("utf-8")
-- set_rundir(".")
add_includedirs("include")
add_installfiles("(assets/**)", {prefixdir = "bin"})

if is_plat("windows") then
    add_cxflags("/wd4251")
end

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

target("Main", function ()
	set_kind("static")
	add_headerfiles("include/(Main/**.hpp)", "include/(Main/**.inl)")
	add_files("src/Main/**.cpp")
	add_packages("spdlog", { public = true })
end)

target("Editor", function ()
    set_kind("binary")
    add_defines("WITH_SCE_EDITOR")
    add_headerfiles("include/DuoBoloShared/**.hpp","include/DuoBoloShared/**.inl", "include/DuoBoloShared/**.h")
    add_headerfiles("include/DuoBoloClient/**.hpp","include/DuoBoloClient/**.inl", "include/DuoBoloClient/**.h")
    add_files("src/DuoBoloClient/**.cpp", "src/DuoBoloShared/**.cpp")
    add_deps("rlimgui", "rcamera")
    add_deps("Main")
end)

target("DuoBoloClient", function ()
    set_kind("binary")
    add_headerfiles("include/DuoBoloShared/**.hpp","include/DuoBoloShared/**.inl", "include/DuoBoloShared/**.h")
    add_headerfiles("include/DuoBoloClient/**.hpp","include/DuoBoloClient/**.inl", "include/DuoBoloClient/**.h")
    add_files("src/DuoBoloClient/**.cpp", "src/DuoBoloShared/**.cpp")
    add_packages("raylib", "enet6", "spdlog", "entt", "nlohmann_json", "argparse", { public = true })
    add_deps("Main", "DuoBoloEngine")
    add_deps("rlimgui", "rcamera")
end)

target("DuoBoloServer", function ()
    set_kind("binary")
    add_defines("IS_SERVER")
    add_headerfiles("include/DuoBoloShared/**.hpp","include/DuoBoloShared/**.inl", "include/DuoBoloShared/**.h")
    add_headerfiles("include/DuoBoloServer/**.hpp","include/DuoBoloServer/**.inl", "include/DuoBoloServer/**.h")
    add_files("src/DuoBoloServer/**.cpp", "src/DuoBoloShared/**.cpp")
    add_packages("entt", "enet6", "nlohmann_json", "raylib", "argparse")
    add_deps("Main", "DuoBoloEngine")
end)

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