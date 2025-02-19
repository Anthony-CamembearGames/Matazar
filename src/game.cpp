#include "game.h"

Game::Game() {}

Game::~Game() {}

bool Game::Initialize() {
 
  mVkrenderer = new VulkanRenderer;
  mVkrenderer->init();
  return true;
}

void Game::Shutdown() {
  mVkrenderer->shutdown();
}

void Game::RunLoop() {
 /* while (mIsRunning) {
    ProcessInput();
    // UpdateGame();
    GenerateOutput();
  }*/
mVkrenderer->loop();

}
void Game::GenerateOutput() {

  // GLenum error = glGetError();
  // std::cout << error << std::endl;
  //  if (error != 0) {
  //  SDL_Log("error" + error);
  //  }
  //  SDL_Log("TestSDL_Log");
  /*glClearColor(0.0f, 0.56f, 0.86f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(mWindow);
  */
}


