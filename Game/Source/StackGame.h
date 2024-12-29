#pragma once
#include "Game.h"
#include "Pointers.h"
#include "ECS/Entity.h"
#include "Cores/TestCore.h"

class StackGame
	: public Game
{
public:
	StackGame( int argc, char** argv );
	~StackGame();

	virtual void OnInitialize() override;

	virtual void OnStart() override;
	virtual void OnUpdate( const UpdateContext& inUpdateContext ) override;
	virtual void OnEnd() override;

    virtual void PreRender() override;
    virtual void PostRender() override;

	bool showDebugStuff = false;
};
