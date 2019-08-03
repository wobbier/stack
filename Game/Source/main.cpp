#include "StackGame.h"
#include "Engine/Engine.h"
#include "ComponentRegistry.h"
int main()
{
	GetEngine().Init(new StackGame());
	GetEngine().Run();

	return 0;
}