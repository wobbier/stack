#include "StackGame.h"
#include "Engine/Engine.h"
#include "ECS/Component.h"
#include "Engine/Clock.h"
#include "Components/Transform.h"
#include "ECS/Entity.h"
#include <string>
#include "Engine/Input.h"
#include "Components/Animation.h"
#include "Components/Camera.h"
#include "Components/Physics/Rigidbody.h"
#include "Components/Graphics/Model.h"
#include "Components/Lighting/Light.h"
#include "Components/Cameras/FlyingCamera.h"
#include "Cores/TestCore.h"
#include <memory>
#include "Engine/World.h"
#include "Path.h"
#include "Game.h"
#include "HavanaEvents.h"

StackGame::StackGame()
	: Game()
{
}

StackGame::~StackGame()
{
}

void StackGame::OnStart()
{
}

void StackGame::OnUpdate(float DeltaTime)
{
	Input& Instance = Input::GetInstance();
}

void StackGame::OnEnd()
{
}

void StackGame::OnInitialize()
{
	NewSceneEvent evt;
	evt.Fire();
	GetEngine().GetWorld().lock()->Start();
	GetEngine().LoadScene("Assets/Test.lvl");
	GetEngine().GetWorld().lock()->Simulate();
	GetEngine().GetWorld().lock()->Start();
}

void StackGame::PostRender()
{
}