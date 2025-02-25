import platform
import os

os.system("glslc shaders/simple_shader.vert -o shaders/vert.spv")

os.system("glslc shaders/simple_shader.frag -o shaders/frag.spv")


env = Environment(COMPILATIONDB_USE_ABSPATH=True)
env.Append(CXXFLAGS="-std=c++17")
env["STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME"] = 1

if platform.system() == "Windows":
    env = Environment(
        CPPPATH=[
            #"C:/VulkanSDK/glfw/include/",
            "C:/VulkanSDK/1.4.304.0/Include/",
            #"C:/VulkanSDK/lglm/",
            "C:/VulkanSDK/sdl/include/",
            #"C:/VulkanSDK/glew-2.1.0/include/"
        ]
    )

    env["LIBPATH"] = [
        #"C:/VulkanSDK/glfw/lib-mingw-w64/",
        "C:/VulkanSDK/1.4.304.0/Lib/",
        "C:/VulkanSDK/sdl/lib/",
        #"C:/VulkanSDK/glew-2.1.0/lib/"
    ]

    #env["LIBS"] = ["glfw3", "glu32","glew32", "opengl32", "vulkan-1", "gdi32", "SDL3"]
    env["LIBS"] = ["vulkan-1", "SDL3"]
else:
    env = Environment(
        CPPPATH=[
        "/usr/include"

        ]
    )
    env["LIBS"] = ["SDL3", "vulkan"]

env.Tool('compilation_db')
env.CompilationDatabase()
env.Program("program", Glob("src/*.cpp"))

#env.Program("program", Glob("src/externals/*.cpp"))
