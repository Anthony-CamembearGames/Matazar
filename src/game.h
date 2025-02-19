#pragma once
#include "vulkanrender.h"

//#include "vertexarray.h"
// #include <GL/glew.h>

class Game {
public:
  Game();
  ~Game();
  bool Initialize();
  void RunLoop();
  void Shutdown();

private:
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();

  bool mIsRunning;
  VulkanRenderer* mVkrenderer;
};
