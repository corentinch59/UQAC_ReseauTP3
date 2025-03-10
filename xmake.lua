add_rules("mode.debug", "mode.release")

includes("xmake/magnum.lua")
includes("xmake/magnum-integration.lua")

add_requires("enet6", "entt", "spdlog", "bullet3", "imgui", "glm")
add_requires("magnum", { configs = { glfw=true }})
add_requires("magnum-integration", { configs = { bullet=true, imgui=true, glm=true}})

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
    add_packages("imgui", "glm", "bullet3", "magnum", "magnum-integration", "enet6", "spdlog", "entt")