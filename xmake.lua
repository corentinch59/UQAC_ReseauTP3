add_rules("mode.debug", "mode.release")

-- Shitty fix for shitty bug when using spdlog and raylib on windows
if is_plat("windows") then
    add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN", "NOGDI", "NOUSER", {public = true})
end

add_requires("enet6", "entt", "spdlog", "bullet3", "imgui", "raylib")

set_project("DuoBoloNetwork")

set_languages("cxx20")
set_exceptions("cxx")
set_encodings("utf-8")
set_rundir(".")
add_includedirs("include")
add_installfiles("assets/**", {prefixdir = "bin"})

add_cxflags("/wd4251")

target("UQAC_ReseauTP3")
    set_kind("binary")
    add_files("src/DuoBoloNetwork/*.cpp")
    add_files("src/main.cpp")
    add_headerfiles("include/DuoBoloNetwork/**.hpp","include/DuoBoloNetwork/**.inl", "include/DuoBoloNetwork/**.h")
    add_packages("imgui", "bullet3", "raylib", "enet6", "spdlog", "entt")
    after_build(function (target)
            os.cp("assets", path.join(target:targetdir(), "assets"))
    end)