add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("iceblcokmc https://github.com/IceBlcokMC/xmake-repo.git")

if is_config("target_type", "server") then
    add_requires("levilamina 1.9.0", {configs = {target_type = "server"}})

    add_requires("economy_bridge 0.2.0")
else
    add_requires("levilamina 1.9.0", {configs = {target_type = "client"}})
end

add_requires("levibuildscript")
add_requires("abseil 20250127.0")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

option("target_type")
    set_default("server")
    set_showmenu(true)
    set_values("server", "client")
option_end()

target("FastMiner") -- Change this to your mod name.
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")
    add_cxflags(
        "/EHa",
        "/utf-8",
        "/W4",
        "/w44265",
        "/w44289",
        "/w44296",
        "/w45263",
        "/w44738",
        "/w45204"
    )
    add_defines("NOMINMAX", "UNICODE", "PLUGIN_NAME=\"FastMiner\"")
    add_files("src/**.cc")
    add_includedirs("src")
    add_packages("levilamina")
    set_exceptions("none") -- To avoid conflicts with /EHa.
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")
    add_packages("abseil")
    add_headerfiles("src/**.h")

    if is_mode("debug") then
        add_defines("DEBUG")
    end

    if is_mode("release") then 
        set_optimize("fastest")
    end 

    if is_config("target_type", "server") then
        add_defines("LL_PLAT_S")
        add_packages("economy_bridge")
        add_files("src-server/**.cc")
        add_includedirs("src-server")
        add_headerfiles("src-server/**.h")
    else
        add_defines("LL_PLAT_C")
        add_files("src-client/**.cc")
        add_includedirs("src-client")
        add_headerfiles("src-client/**.h")
    end