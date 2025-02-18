#include "game.h"
#include <iostream>

int main(int argc, char *argv[]) {

  Game game;

  bool gameIsInitialised = game.Initilize();

  std::cout << std::boolalpha;
  std::cout << "  Game is initialised:   " << gameIsInitialised << std::endl;

  if (gameIsInitialised) {

    game.RunLoop();
  }

  game.Shutdown();
  return 0;
}
