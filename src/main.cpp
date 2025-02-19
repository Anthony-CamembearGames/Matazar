#include "game.h"
#include <iostream>

int main(int argc, char *argv[]) {

  Game game;

  bool gameIsInitialised = game.Initialize();


  if (gameIsInitialised) {

    game.RunLoop();
  }

  game.Shutdown();
  return 0;
}
