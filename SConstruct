import platform

env = Environment(COMPILATIONDB_USE_ABSPATH=True)
env.Append(CXXFLAGS="-std=c++17")
env["STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME"] = 1

if platform.system() == "Windows":
    env = Environment(
        CPPPATH=[
            "C:/VulkanSDK/glfw/include/",
            "C:/VulkanSDK/1.4.304.0/Include/",
            "C:/VulkanSDK/lglm/",
            "C:/VulkanSDK/sdl/include/",
        ]
    )

    env["LIBPATH"] = [
        "C:/VulkanSDK/glfw/lib-mingw-w64/",
        "C:/VulkanSDK/1.4.304.0/Lib/",
        "C:/VulkanSDK/sdl/lib/",
    ]

    env["LIBS"] = ["glfw3", "vulkan-1", "gdi32", "SDL3"]
else:
    env["LIBS"] = ["SDL3"]

env.Tool('compilation_db')
env.CompilationDatabase()
env.Program("program", Glob("src/*.cpp"))
