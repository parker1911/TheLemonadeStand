#include "GameManager.h"

int main(int argc, char* args[])
{
	GameManager gameManager;

	if (gameManager.Init()){
		gameManager.Update();
	}

	gameManager.Cleanup();

	return 0;
}