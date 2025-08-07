add_rules("mode.debug", "mode.release")

add_requires("volk", "vulkan-headers")

set_languages("c++20")

target("tmp")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("volk", "vulkan-headers")



