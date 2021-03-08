#include "APCore.h"
#include "Components/Physics/Rigidbody.h"
#include "Graphics/Texture.h"
#include "Resource/ResourceCache.h"
#include "Components/GameUIView.h"
#include "Components/Audio/AudioSource.h"
#include "Mathf.h"
#include "Cores/Utility/SelfDestructCore.h"
#include <stdlib.h>
#include "Math/Random.h"
#include "Events/SceneEvents.h"
#include "Components/StackBlock.h"
#include "Cores/SceneGraph.h"
#include "Components/Graphics/Model.h"
#include "Engine/Engine.h"
#include "Components/Camera.h"
#include "Components/Graphics/Mesh.h"
#include "Primitives/Plane.h"
#include "StreakFXCore.h"

static int testColors = 0;

APCore::APCore()
	: Base(ComponentFilter().Requires<Transform>().Requires<StackBlock>())
	, m_gridSnapSize(kStartDistance, kStartDistance)
	, m_currentStackSize(2.f, 2.f, 2.f)
{

}

void APCore::OnEntityAdded(Entity& NewEntity)
{

}

void APCore::OnEntityRemoved(Entity& InEntity)
{

}

void APCore::Init()
{

}

void APCore::OnStart()
{
	testColors = 0;

	{
		SceneGraph* graph = GetEngine().SceneNodes;
		Transform* uiEnt = graph->RootTransform->GetComponent<Transform>().GetChildByName("UI");
		if (!uiEnt)
		{
			return;
		}
		m_uiScore = uiEnt->Parent;
	}

	m_currentBlock = GetWorld().CreateEntity();
	Transform& prevTransform = m_currentBlock->AddComponent<Transform>("Root StackBlock");
	prevTransform.SetScale(Vector3(2.f, 8.f, 2.f));
	prevTransform.Translate(Vector3(0.f, -6.f, 0.f));
	StackBlock& prevBlock = m_currentBlock->AddComponent<StackBlock>();
	prevBlock.BlockMoveSpeed = 0.f;
	m_currentBlock->AddComponent<Model>("Assets/Cube.fbx");
	//m_currentBlock->AddComponent<Mesh>(MeshType::Cube);
	GenerateNextHue();

	Camera::CurrentCamera->ClearColor = Darken(GetHue(testPercent / 100.f), 0.20f);

	prevBlock.Color = GetHue(testPercent / 100.f);
	if (m_currentBlock->HasComponent<Mesh>())
	{
		Mesh& mesh = m_currentBlock->GetComponent<Mesh>();
		mesh.MeshMaterial->DiffuseColor = GetHue(blockPercent / 100.f);
		mesh.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/CubeDiffuse.jpg")));
		mesh.MeshMaterial->SetTexture(Moonlight::TextureType::Opacity, ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/BaseCubeDiffuse.jpg")));
	}

	SpawnNextBlock();

	SetupCamera();
}

void APCore::OnStop()
{
	ClearBlocks();
}

void APCore::Update(float dt)
{
	if (m_currentBlock && m_state != GameState::Lost)
	{
		Transform& transform = m_currentBlock->GetComponent<Transform>();
		StackBlock& block = m_currentBlock->GetComponent<StackBlock>();
		m_currentPosition = transform.GetPosition();
		if (!block.MoveOnX)
		{
			if (block.BlockDirection)
			{
				if (m_currentPosition.z < kStartDistance)
				{
					m_currentPosition.z = (m_currentPosition.z + (block.BlockMoveSpeed * dt));
				}
				else
				{
					block.BlockDirection = false;
				}
			}
			else
			{
				if (m_currentPosition.z > -kStartDistance)
				{
					m_currentPosition.z = (m_currentPosition.z - (block.BlockMoveSpeed * dt));
				}
				else
				{
					block.BlockDirection = true;
				}
			}

		}
		else
		{
			if (block.BlockDirection)
			{
				if (m_currentPosition.x < kStartDistance)
				{
					m_currentPosition.x = (m_currentPosition.x + (block.BlockMoveSpeed * dt));
				}
				else
				{
					block.BlockDirection = false;
				}
			}
			else
			{
				if (m_currentPosition.x > -kStartDistance)
				{
					m_currentPosition.x = (m_currentPosition.x - (block.BlockMoveSpeed * dt));
				}
				else
				{
					block.BlockDirection = true;
				}
			}
		}
		transform.SetPosition(m_currentPosition);
	}

	m_totalTime += dt;
	float distCovered = (m_totalTime - m_startTime) * m_cameraFocusSpeed;
	m_fracJourney = distCovered / m_cameraTravelDistance;

	Transform& camTransform = m_mainCamera->GetComponent<Transform>();
	if (m_fracJourney < 1.0f)
	{
#if ME_PLATFORM_UWP || ME_PLATFORM_WIN64
		camTransform.SetPosition(Mathf::Lerp(camTransform.GetPosition(), Vector3(camTransform.GetPosition().x, m_currentPosition.y + m_cameraHeightOffset, camTransform.GetPosition().z), m_fracJourney));
#endif
	}

	auto input = GetEngine().GetInput();
	if (!m_isKeyPressed && (input.IsKeyDown(KeyCode::P)/* || Controller.buttons.y)*/))
	{
		auto& entities = GetEntities();
		for (Entity& entity : entities)
		{
			if (entity.HasComponent<Rigidbody>())
			{
				entity.GetComponent<Rigidbody>().SetMass(1.0f);
			}
		}
		m_isKeyPressed = false;
	}
	if (!m_isKeyPressed && (input.IsKeyDown(KeyCode::Space) /*|| Controller.buttons.a*/) /*|| testColors <= 40*/)
	{
		switch (m_state)
		{
		case GameState::Start:
			//OnStart();
			m_state = GameState::Active;
		case GameState::Active:
		{
			testColors++;
			bool canContinue = true;
			if (m_previousBlock)
			{
				canContinue = EndBlock();
			}
			if (canContinue)
			{
				SpawnNextBlock();
			}
			break;
		}
		case GameState::Lost:
		{
			m_state = GameState::Restart;
		}
		case GameState::Restart:
			ClearBlocks();
			OnStart();
			m_state = GameState::Start;
			break;
		default:
			break;

		}
		m_isKeyPressed = false;
	}
	m_isKeyPressed = (input.IsKeyDown(KeyCode::Space)/* || Controller.buttons.a*/);
}

void APCore::SpawnNextBlock()
{
	UpdateScore();
	m_previousBlock = m_currentBlock;
	Transform& prevTransform = m_previousBlock->GetComponent<Transform>();
	StackBlock& prevBlock = m_previousBlock->GetComponent<StackBlock>();
	Vector3 prevScale = prevTransform.GetScale();
	Rigidbody& rigidbody = m_previousBlock->AddComponent<Rigidbody>(Rigidbody::ColliderType::Box);
	rigidbody.SetScale(prevTransform.GetScale());
	rigidbody.SetMass(0.0f);

	m_currentBlock = GetWorld().CreateEntity();
	Transform& transform = m_currentBlock->AddComponent<Transform>(std::string("StackBlock " + std::to_string(GetEntities().size())));
	StackBlock& block = m_currentBlock->AddComponent<StackBlock>();
	Model& model = m_currentBlock->AddComponent<Model>("Assets/Cube.fbx");

	m_currentStackSize.y = .3f;
	transform.SetScale(m_currentStackSize);

	block.MoveOnX = !prevBlock.MoveOnX;
	if (!block.MoveOnX)
	{
		transform.SetPosition(prevTransform.GetPosition() + Vector3(0.f, (prevTransform.GetScale().y) + (.3f), -kStartDistance));
		block.BlockDirection = !block.BlockDirection;
	}
	else
	{
		transform.SetPosition(prevTransform.GetPosition() + Vector3(kStartDistance, (prevTransform.GetScale().y) + (.3f), 0.f));
	}

	if (transform.Parent->HasComponent<Mesh>())
	{
		Mesh& mesh = transform.Parent->GetComponent<Mesh>();
		GenerateNextHue();


		Camera::CurrentCamera->ClearColor = Darken(GetHue(testPercent / 100.f), 0.20f);

		mesh.MeshMaterial->DiffuseColor = Darken(GetHue(blockPercent / 100.f), 0.10f);
		block.Color = mesh.MeshMaterial->DiffuseColor;
	}

	{
		// Keep a note of the time the movement started.
		m_startTime = 0.f;

		// Calculate the journey length.
		m_cameraTravelDistance = (prevTransform.GetPosition() - transform.GetPosition()).Length();

		m_totalTime = 0.f;
	}
	GetWorld().Simulate();
}

void APCore::SetupCamera()
{
	Transform* cameraEnt = GetEngine().SceneNodes->RootTransform->GetComponent<Transform>().GetChildByName("Main Camera");
	m_mainCamera = cameraEnt->Parent;

	cameraEnt->SetPosition(Vector3(-5.f, m_currentBlock->GetComponent<Transform>().GetPosition().y + 5, 5.f));
	cameraEnt->SetRotation(Vector3(0.f, 45.f, 0.f));
	Camera& cam = m_mainCamera->GetComponent<Camera>();
	cam.Projection = Moonlight::ProjectionType::Orthographic;
	cameraEnt->LookAt((Vector3(0.f, 0.f, 0.f) - cameraEnt->GetPosition()).Normalized());

	cameraEnt->SetPosition(cameraEnt->GetPosition() + Vector3(0.f, 10.f, 0.f));
}

bool APCore::EndBlock()
{
	Transform& prevTransform = m_previousBlock->GetComponent<Transform>();
	Vector3& prevPos = prevTransform.GetPosition();

	Transform& transform = m_currentBlock->GetComponent<Transform>();
	StackBlock& block = m_currentBlock->GetComponent<StackBlock>();
	Vector3& pos = transform.GetPosition();

	if (block.MoveOnX)
	{
		float distanceFailedX = prevPos.x - pos.x;
		float distAbs = Mathf::Abs(distanceFailedX);
		if (distAbs > kErrorMargin)
		{
			float scaleThing = m_currentStackSize.x - (distAbs / 2.f);
			float remaining = m_currentStackSize.x - scaleThing;
			m_currentStackSize.x = scaleThing;

			if (m_currentStackSize.x <= 0.f)
			{
				Reset(transform);
				LoseGame();
				return false;
			}
			transform.SetScale(m_currentStackSize);

			float middle = prevTransform.GetPosition().x + (transform.GetPosition().x / 2.f);
			float finalPosX = middle - (prevTransform.GetPosition().x / 2.f);

			pos.z = prevTransform.GetPosition().z;

			float direction = finalPosX - prevTransform.GetPosition().x;
			if (direction > 0)
			{
				pos.x = (finalPosX + transform.GetScale().x + remaining);
			}
			else
			{
				pos.x = (finalPosX - transform.GetScale().x - remaining);
			}

			CreateBrokenPiece(remaining, pos, block.MoveOnX, (direction > 0));
			pos.x = finalPosX;
		}
		else
		{
			if (m_currentStackSize.x > kBoundsLimit)
			{
				LoseGame();
				return false;
			}
			pos.x = prevPos.x;
			pos.z = prevPos.z;
			m_currentStreak++;
			AddStreakFX(pos, m_currentStreak);
		}
	}
	else
	{
		float distanceFailedZ = prevPos.z - pos.z;
		float distAbs = Mathf::Abs(distanceFailedZ);
		if (distAbs > kErrorMargin)
		{
			float scaleThing = m_currentStackSize.z - (distAbs / 2.f);
			float remaining = m_currentStackSize.z - scaleThing;
			m_currentStackSize.z = scaleThing;

			if (m_currentStackSize.z <= 0.f)
			{
				Reset(transform);
				LoseGame();
				return false;
			}
			transform.SetScale(m_currentStackSize);

			float middle = prevTransform.GetPosition().z + (transform.GetPosition().z / 2.f);
			float finalPosZ = middle - (prevTransform.GetPosition().z / 2.f);

			pos.x = prevTransform.GetPosition().x;
			float direction = finalPosZ - prevTransform.GetPosition().z;
			if (direction > 0)
			{
				pos.z = (finalPosZ + transform.GetScale().z + remaining);
			}
			else
			{
				pos.z = (finalPosZ - transform.GetScale().z - remaining);
			}
			CreateBrokenPiece(remaining, pos, block.MoveOnX, (direction > 0));

			pos.z = (finalPosZ);
		}
		else
		{
			if (m_currentStackSize.z > kBoundsLimit)
			{
				LoseGame();
				return false;
			}
			pos.x = prevPos.x;
			pos.z = prevPos.z;
			m_currentStreak++;
			AddStreakFX(pos, m_currentStreak);
		}
	}

	m_mainCamera->GetComponent<AudioSource>().Play();

	transform.SetPosition(pos);
	return true;
}

void APCore::CreateBrokenPiece(float amountLost, Vector3 position, bool blockMovingOnX, bool PositiveDirection)
{
	m_currentStreak = 0;
	StackBlock block = m_currentBlock->GetComponent<StackBlock>();

	EntityHandle broken = GetWorld().CreateEntity();
	Transform& transform = broken->AddComponent<Transform>("Broken Piece " + std::to_string(GetEntities().size()));
	Vector3 stack = m_currentStackSize;
	if (block.MoveOnX)
	{
		stack.x = Mathf::Abs(amountLost);
	}
	else
	{
		stack.z = Mathf::Abs(amountLost);
	}
	transform.SetPosition(position);
	transform.SetScale(stack);
	transform.SetRotation(Vector3());

	broken->AddComponent<Model>("Assets/Cube.fbx");
	broken->AddComponent<SelfDestruct>(5.f);
	Rigidbody& rigidbody = broken->AddComponent<Rigidbody>(Rigidbody::ColliderType::Box);
	rigidbody.SetScale(stack);
	if (blockMovingOnX)
	{
		if (PositiveDirection)
		{
			rigidbody.SetVelocity(Vector3(1.f, 0.f, 0.f));
		}
		else
		{
			rigidbody.SetVelocity(Vector3(-1.f, 0.f, 0.f));
		}
	}
	else
	{
		if (PositiveDirection)
		{
			rigidbody.SetVelocity(Vector3(0.f, 0.f, 1.f));
		}
		else
		{
			rigidbody.SetVelocity(Vector3(0.f, 0.f, -1.f));
		}
	}
	{
		//child->Reset();
		if (broken->HasComponent<Mesh>())
		{
			Mesh& mesh = broken->GetComponent<Mesh>();
			mesh.MeshMaterial->DiffuseColor = block.Color;
			block.Color = mesh.MeshMaterial->DiffuseColor;
		}
	}
}

void RecusiveDeleteBlock(Entity& ent, Transform* trans)
{
	if (!trans)
	{
		return;
	}
	for (auto child : trans->GetChildren())
	{
		RecusiveDeleteBlock(*child->Parent.Get(), child.get());
	}
	ent.MarkForDelete();
};

void APCore::Reset(Transform& transform)
{
	m_currentStackSize.x = 2.f;
	m_currentStackSize.z = 2.f;
}

unsigned int APCore::UpdateScore()
{
	size_t ents = GetEntities().size();
	unsigned int Score = static_cast<unsigned int>(ents > 0 ? ents - 1 : 0);

	if (m_uiScore)
	{
        
#if ME_PLATFORM_UWP || ME_PLATFORM_WIN64
		// #TODO This could be any other BasicUIView
		GameUIView& view = m_uiScore->GetComponent<GameUIView>();
#endif
		//view.UpdateScore(Score);
	}
	return Score;
}

void APCore::LoseGame()
{
	m_currentStackSize.x = 2.f;
	if (m_uiScore)
	{
        
#if ME_PLATFORM_UWP || ME_PLATFORM_WIN64
		// #TODO This could be any other BasicUIView
		GameUIView& view = m_uiScore->GetComponent<GameUIView>();
#endif
		//view.SetMessage("uh oh");
		m_currentBlock->AddComponent<Rigidbody>();
		//OnStop();
	}
	m_state = GameState::Lost;
}

Vector3 APCore::Darken(const Vector3& OutColor, float percent)
{
	return Vector3(OutColor.x - OutColor.x * percent, OutColor.y - OutColor.y * percent, OutColor.z - OutColor.z * percent);
}


Vector3 APCore::GetHue(float percent)
{
	int normalized = static_cast<int>(percent * 256.f * 6.f);
	int x = normalized % 256;

	int red = 0;
	int green = 0;
	int blue = 0;
	switch (normalized / 256)
	{
	// Red
	case 0:
		red = 255;
		green = x;
		break;
	// Yellow
	case 1:
		red = 255 - x;
		green = 255;
		break;
	// Green
	case 2:
		green = 255;
		blue = x;
		break;
	// Cyan
	case 3:
		green = 255 - x;
		blue = 255;
		break;
	// Blue
	case 4:
		red = x;
		blue = 255;
		break;
	// Magenta
	case 5:
		red = 255;
		blue = 255 - x;
		break;
	}

	return Vector3(red / 255.f, green / 255.f, blue / 255.f);
}

void APCore::GenerateNextHue()
{
	if (!initialized && testPercent == 0)
	{
		testPercent = random(0, 100);
		initialized = true;
	}

	testPercent += 1.f;
	blockPercent = testPercent - 50.f;

	if (testPercent >= 100)
	{
		testPercent = 0;
	}

	if (blockPercent < 0.f)
	{
		blockPercent += 100;
	}
}

void APCore::ClearBlocks()
{
	for (auto ent : GetEntities())
	{
		//ent.MarkForDelete();
		RecusiveDeleteBlock(ent, &ent.GetComponent<Transform>());
	}
	GetWorld().Simulate();
}

void APCore::AddStreakFX(Vector3 pos, int streakNum)
{
	auto plane = GetWorld().CreateEntity();
	plane->AddComponent<SelfDestruct>(0.3f);
	Vector3 currentScale = m_currentBlock->GetComponent<Transform>().GetScale();
	pos.y -= (currentScale.y);
	Transform& transform = plane->AddComponent<Transform>();
	transform.SetPosition(pos);
	currentScale.y = 1.f;
	transform.SetScale(currentScale);
	plane->AddComponent<StreakFX>();
	Mesh& mesh = plane->AddComponent<Mesh>(new PlaneMesh());
	SharedPtr<Moonlight::Texture> tex;
	if (streakNum == 1)
	{
		tex = ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/Textures/Streak1.png"));
	}
	else
	{
		tex = ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/Textures/Streak2.png"));
	}

	mesh.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, tex);
	mesh.MeshMaterial->SetTexture(Moonlight::TextureType::Opacity, tex);
}


#if ME_EDITOR

void APCore::OnEditorInspect()
{
	Base::OnEditorInspect();
	ImGui::DragFloat("Camera Height Offset", &m_cameraHeightOffset);
	ImGui::DragFloat2("Grid Size", &m_gridSnapSize[0]);
}

#endif
