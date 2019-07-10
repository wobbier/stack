#pragma once
#include "Game.h"
#include "Pointers.h"
#include "ECS/Entity.h"
#include "Cores/TestCore.h"

class StackGame
	: public Game
{
public:
	StackGame();
	~StackGame();

	virtual void OnInitialize() override;

	virtual void OnStart() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnEnd() override;

	virtual void PostRender() override;

	class FlyingCameraCore* FlyingCameraController = nullptr;
	class PhysicsCore* Physics = nullptr;

	WeakPtr<Entity> MainCamera;
	WeakPtr<Entity> SecondaryCamera;
};