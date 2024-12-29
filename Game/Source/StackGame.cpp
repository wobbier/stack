#include "StackGame.h"
#include "Engine/Engine.h"
#include "ECS/Component.h"
#include "Engine/Clock.h"
#include "Components/Transform.h"
#include "ECS/Entity.h"
#include <string>
#include "Engine/Input.h"
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
#include "Events/SceneEvents.h"
#include "CLog.h"

StackGame::StackGame( int argc, char** argv )
	: Game( argc, argv )
{
}

StackGame::~StackGame()
{
}

void StackGame::OnStart()
{
}

void StackGame::OnUpdate( const UpdateContext& inUpdateContext )
{
	if ( showDebugStuff )
	{
		ImGui::ShowDemoWindow( &showDebugStuff );
	}

	if ( GetEngine().GetInput().IsKeyDown( KeyCode::A ) || GetEngine().GetInput().GetMousePosition().x > 0 )
	{
		int i = 0;
		++i;
	}
}

void StackGame::OnEnd()
{
}

void StackGame::OnInitialize()
{
	GetEngine().LoadScene( "Assets/Test.lvl" );
}

void StackGame::PreRender()
{
}

void StackGame::PostRender()
{
}
