#include "StackGame.h"
#include "Engine/Engine.h"

int main()
{
	Engine gameEngine;

	gameEngine.Init(new StackGame());
	gameEngine.Run();

	return 0;
}