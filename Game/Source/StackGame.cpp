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
#include "Cores/PhysicsCore.h"
#include "Cores/Cameras/FlyingCameraCore.h"

StackGame::StackGame()
	: Game()
{
	Physics = new PhysicsCore();
	FlyingCameraController = new FlyingCameraCore();
}

StackGame::~StackGame()
{
}

void StackGame::OnStart()
{
	GetEngine().LoadScene("Assets/Alley.lvl");
	auto GameWorld = GetEngine().GetWorld().lock();

	MainCamera = GameWorld->CreateEntity();
	Transform& CameraPos = MainCamera.lock()->AddComponent<Transform>("Main Camera");
	CameraPos.SetPosition(Vector3(0, 5, 20));
	Camera& cam = MainCamera.lock()->AddComponent<Camera>();
	MainCamera.lock()->AddComponent<FlyingCamera>();
	MainCamera.lock()->AddComponent<Light>();

	SecondaryCamera = GameWorld->CreateEntity();
	Transform& SecondaryPos = SecondaryCamera.lock()->AddComponent<Transform>("Secondary Camera");
	SecondaryPos.SetPosition(Vector3(0, 5, 20));
	SecondaryCamera.lock()->AddComponent<Camera>();
	SecondaryCamera.lock()->AddComponent<Light>();
	SecondaryCamera.lock()->AddComponent<FlyingCamera>();

	auto TestModel = GameWorld->CreateEntity();
	Transform& ModelTransform = TestModel.lock()->AddComponent<Transform>("Sponza");
	ModelTransform.SetPosition(Vector3(0.f, 0.f, 0.f));
	ModelTransform.SetScale(Vector3(.1f, .1f, .1f));
	//TestModel.lock()->AddComponent<Rigidbody>();
	//TestModel.lock()->AddComponent<Model>("Assets/ExampleAssets/Models/Hammer.fbx");
	TestModel.lock()->AddComponent<Model>("Assets/Craftsman/Craftsman.fbx");

	FlyingCameraController->SetCamera(&cam);
	GameWorld->AddCore<FlyingCameraCore>(*FlyingCameraController);
	GameWorld->AddCore<PhysicsCore>(*Physics);
}

void StackGame::OnUpdate(float DeltaTime)
{
	Physics->Update(DeltaTime);

	FlyingCameraController->Update(DeltaTime);

	Input& Instance = Input::GetInstance();
	if (Instance.IsKeyDown(KeyCode::Number1))
	{
		MainCamera.lock()->GetComponent<Camera>().SetCurrent();
	}
	if (Instance.IsKeyDown(KeyCode::Number2))
	{
		SecondaryCamera.lock()->GetComponent<Camera>().SetCurrent();
	}
}

void StackGame::OnEnd()
{
}

void StackGame::OnInitialize()
{
}

void StackGame::PostRender()
{
}