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

#if ME_EDITOR
void APCore::OnEditorInspect()
{
	Base::OnEditorInspect();
	ImGui::DragFloat("Camera Height Offset", &m_cameraHeightOffset);
	ImGui::DragFloat2("Grid Size", &m_gridSnapSize[0]);
}
#endif

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
				if (m_currentPosition.Z() < kStartDistance)
				{
					m_currentPosition.SetZ(m_currentPosition.Z() + (block.BlockMoveSpeed * dt));
				}
				else
				{
					block.BlockDirection = false;
				}
			}
			else
			{
				if (m_currentPosition.Z() > -kStartDistance)
				{
					m_currentPosition.SetZ(m_currentPosition.Z() - (block.BlockMoveSpeed * dt));
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
				if (m_currentPosition.X() < kStartDistance)
				{
					m_currentPosition.SetX(m_currentPosition.X() + (block.BlockMoveSpeed * dt));
				}
				else
				{
					block.BlockDirection = false;
				}
			}
			else
			{
				if (m_currentPosition.X() > -kStartDistance)
				{
					m_currentPosition.SetX(m_currentPosition.X() - (block.BlockMoveSpeed * dt));
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
		camTransform.SetPosition(Mathf::Lerp(camTransform.GetPosition(), Vector3(camTransform.GetPosition().X(), m_currentPosition.Y() + m_cameraHeightOffset, camTransform.GetPosition().Z()), m_fracJourney));
	}
	auto Keyboard = GetEngine().GetInput().GetKeyboardState();
	auto Controller = GetEngine().GetInput().GetControllerState();
	if (!m_isKeyPressed && (Keyboard.P || Controller.buttons.y))
	{
		auto entities = GetEntities();
		for (Entity& entity : entities)
		{
			if (entity.HasComponent<Rigidbody>())
			{
				entity.GetComponent<Rigidbody>().SetMass(1.0f);
			}
		}
		m_isKeyPressed = false;
	}
	if (!m_isKeyPressed && (Keyboard.Space || Controller.buttons.a) /*|| testColors <= 40*/)
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
			m_state = GameState::Restart;
			break;
		case GameState::Restart:
			OnStart();
			m_state = GameState::Start;
			break;
		default:
			break;

		}
		m_isKeyPressed = false;
	}
	m_isKeyPressed = (Keyboard.Space || Controller.buttons.a);
}

void APCore::Init()
{

}

void APCore::OnStart()
{
	m_colors.clear();
	while (!m_colorQueue.empty())
	{
		m_colorQueue.pop();
	}
	testColors = 0;
	m_colors.reserve(6);
	m_colors.push_back(Vector3(254, 211, 48) / 255.f);
	m_colors.push_back(Vector3(38, 222, 129) / 255.f);
	m_colors.push_back(Vector3(253, 150, 68) / 255.f);
	m_colors.push_back(Vector3(43, 203, 186) / 255.f);
	m_colors.push_back(Vector3(252, 92, 101) / 255.f);
	m_colors.push_back(Vector3(69, 170, 242) / 255.f);
	{
		SceneGraph* graph = GetEngine().SceneNodes;
		Transform* uiEnt = graph->RootTransform->GetComponent<Transform>().GetChildByName("UI");
		if(!uiEnt)
		{
			return;
		}
		m_uiScore = uiEnt->Parent;
	}

	for (int i = 0; i < 10; ++i)
	{
		GetNextColor();
	}

	m_currentBlock = GetWorld().CreateEntity();
	Transform& prevTransform = m_currentBlock->AddComponent<Transform>("Root StackBlock");
	prevTransform.SetScale(Vector3(2.f, 8.f, 2.f));
	prevTransform.Translate(Vector3(0.f, -6.f, 0.f));
	StackBlock& prevBlock = m_currentBlock->AddComponent<StackBlock>();
	prevBlock.BlockMoveSpeed = 0.f;
	m_currentBlock->AddComponent<Model>("Assets/Cube.fbx");

	GenerateNextHue();

	Camera::CurrentCamera->ClearColor = Darken(GetHue(testPercent / 100.f), 0.20f);

	prevBlock.Color = GetHue(testPercent / 100.f);
	for (SharedPtr<Transform> child : prevTransform.GetChildren())
	{
		child->Reset();
		if (child->Parent->HasComponent<Mesh>())
		{
			m_currentColorStepPercent += m_colorStepPercent;
			Mesh& mesh = child->Parent->GetComponent<Mesh>();
			mesh.MeshMaterial->DiffuseColor = GetHue(blockPercent / 100.f);
			mesh.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/CubeDiffuse.jpg")));
			mesh.MeshMaterial->SetTexture(Moonlight::TextureType::Opacity, ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/BaseCubeDiffuse.jpg")));
		}
	}
	SpawnNextBlock();

	m_currentBlock->SetActive(false);
	SetupCamera();
}

Vector3 APCore::GetNextColor()
{
	Vector3 returnColor = Vector3::Lerp(m_colors[m_currentColorIndex], m_colors[m_targetColorIndex], m_currentColorStepPercent);

	if (m_currentColorStepPercent >= 1.f)
	{
		m_currentColorStepPercent = 0;
		m_currentColorIndex++;
		m_targetColorIndex++;
	}
	else
	{
	}
	if (m_targetColorIndex >= m_colors.size())
	{
		m_targetColorIndex = 0;
	}
	if (m_currentColorIndex >= m_colors.size())
	{
		m_currentColorIndex = 0;
	}
	m_currentColorStepPercent += m_colorStepPercent;
	//BRUH("New Color(" + std::to_string(m_targetColorIndex) + "): " + m_colors[m_targetColorIndex].ToString() + " - Old: " + m_colors[GetPreviousColorIndex()].ToString());
	m_colorQueue.push(returnColor);
	return returnColor;
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

	m_currentStackSize.SetY(.3f);
	transform.SetScale(m_currentStackSize);

	block.MoveOnX = !prevBlock.MoveOnX;
	if (!block.MoveOnX)
	{
		transform.SetPosition(prevTransform.GetPosition() + Vector3(0.f, (prevTransform.GetScale().Y()) + (.3f), -kStartDistance));
		block.BlockDirection = !block.BlockDirection;
	}
	else
	{
		transform.SetPosition(prevTransform.GetPosition() + Vector3(kStartDistance, (prevTransform.GetScale().Y()) + (.3f), 0.f));
	}

	for (SharedPtr<Transform> child : transform.GetChildren())
	{
		child->Reset();
		if (child->Parent->HasComponent<Mesh>())
		{
			Mesh& mesh = child->Parent->GetComponent<Mesh>();
			GenerateNextHue();


			Camera::CurrentCamera->ClearColor = Darken(GetHue(testPercent / 100.f), 0.20f);

			mesh.MeshMaterial->DiffuseColor = Darken(GetHue(blockPercent / 100.f), 0.10f);
			block.Color = mesh.MeshMaterial->DiffuseColor;
			//m_colorQueue.pop();
		}
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

	cameraEnt->SetPosition(Vector3(-5.f, m_currentBlock->GetComponent<Transform>().GetPosition().Y() + 5, 5.f));
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
		float distanceFailedX = prevPos.X() - pos.X();
		float distAbs = Mathf::Abs(distanceFailedX);
		if (distAbs > kErrorMargin)
		{
			float scaleThing = m_currentStackSize.X() - (distAbs / 2.f);
			float remaining = m_currentStackSize.X() - scaleThing;
			m_currentStackSize.SetX(scaleThing);

			if (m_currentStackSize.X() <= 0.f)
			{
				Reset(transform);
				LoseGame();
				return false;
			}
			transform.SetScale(m_currentStackSize);

			float middle = prevTransform.GetPosition().X() + (transform.GetPosition().X() / 2.f);
			float finalPosX = middle - (prevTransform.GetPosition().X() / 2.f);

			pos.SetZ(prevTransform.GetPosition().Z());

			float direction = finalPosX - prevTransform.GetPosition().X();
			if (direction > 0)
			{
				pos.SetX(finalPosX + transform.GetScale().X() + remaining);
			}
			else
			{
				pos.SetX(finalPosX - transform.GetScale().X() - remaining);
			}

			CreateBrokenPiece(remaining, pos);
			pos.SetX(finalPosX);
		}
		else
		{
			if (m_currentStackSize.X() > kBoundsLimit)
			{
				LoseGame();
				return false;
			}
			pos.SetX(prevPos.X());
			pos.SetZ(prevPos.Z());
		}
	}
	else
	{
		float distanceFailedZ = prevPos.Z() - pos.Z();
		float distAbs = Mathf::Abs(distanceFailedZ);
		if (distAbs > kErrorMargin)
		{
			float scaleThing = m_currentStackSize.Z() - (distAbs / 2.f);
			float remaining = m_currentStackSize.Z() - scaleThing;
			m_currentStackSize.SetZ(scaleThing);

			if (m_currentStackSize.Z() <= 0.f)
			{
				Reset(transform);
				LoseGame();
				return false;
			}
			transform.SetScale(m_currentStackSize);

			float middle = prevTransform.GetPosition().Z() + (transform.GetPosition().Z() / 2.f);
			float finalPosZ = middle - (prevTransform.GetPosition().Z() / 2.f);

			pos.SetX(prevTransform.GetPosition().X());
			float direction = finalPosZ - prevTransform.GetPosition().Z();
			if (direction > 0)
			{
				pos.SetZ(finalPosZ + transform.GetScale().Z() + remaining);
			}
			else
			{
				pos.SetZ(finalPosZ - transform.GetScale().Z() - remaining);
			}
			CreateBrokenPiece(remaining, pos);

			pos.SetZ(finalPosZ);
		}
		else
		{
			if (m_currentStackSize.Z() > kBoundsLimit)
			{
				LoseGame();
				return false;
			}
			pos.SetX(prevPos.X());
			pos.SetZ(prevPos.Z());
		}
	}

	m_mainCamera->GetComponent<AudioSource>().Play();

	transform.SetPosition(pos);
	return true;
}

void APCore::CreateBrokenPiece(float amountLost, Vector3 position)
{
	StackBlock block = m_currentBlock->GetComponent<StackBlock>();

	EntityHandle broken = GetWorld().CreateEntity();
	Transform& transform = broken->AddComponent<Transform>("Broken Piece " + std::to_string(GetEntities().size()));
	Vector3 stack = m_currentStackSize;
	if (block.MoveOnX)
	{
		stack.SetX(Mathf::Abs(amountLost));
	}
	else
	{
		stack.SetZ(Mathf::Abs(amountLost));
	}
	transform.SetPosition(position);
	transform.SetScale(stack);
	broken->AddComponent<Model>("Assets/Cube.fbx");
	broken->AddComponent<SelfDestruct>(5.f);
	Rigidbody& rigidbody = broken->AddComponent<Rigidbody>(Rigidbody::ColliderType::Box);
	rigidbody.SetScale(stack);
	for (SharedPtr<Transform> child : transform.GetChildren())
	{
		child->Reset();
		if (child->Parent->HasComponent<Mesh>())
		{
			Mesh& mesh = child->Parent->GetComponent<Mesh>();
			//child->Parent->AddComponent<SelfDestruct>(5.f);
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

void APCore::OnStop()
{
	for (auto ent : GetEntities())
	{
		//ent.MarkForDelete();
		RecusiveDeleteBlock(ent, &ent.GetComponent<Transform>());
	}
	GetWorld().Simulate();
}

void APCore::Reset(Transform& transform)
{
	m_currentStackSize.SetX(2.f);
	m_currentStackSize.SetZ(2.f);
	transform.SetScale(m_currentStackSize);
	Vector3 pos = transform.GetPosition();
	pos[0] = 0.0f;
	pos[2] = 0.0f;
	transform.SetPosition(pos);
}

unsigned int APCore::UpdateScore()
{
	size_t ents = GetEntities().size();
	unsigned int Score = ents;// (ents > 0 ? ents - 1 : 0);

	if (m_uiScore)
	{
		// #TODO This could be any other BasicUIView
		GameUIView& view = m_uiScore->GetComponent<GameUIView>();

		view.UpdateScore(Score);
	}
	return Score;
}

int APCore::GetPreviousColorIndex()
{
	if (m_currentColorIndex <= 0)
	{
		return m_colors.size() - 1;
	}
	else if (m_currentColorIndex >= m_colors.size())
	{
		return 0;
	}
	return m_currentColorIndex - 1;
}

void APCore::LoseGame()
{
	m_currentStackSize.SetX(2.f);
	if (m_uiScore)
	{
		// #TODO This could be any other BasicUIView
		GameUIView& view = m_uiScore->GetComponent<GameUIView>();

		view.SetMessage("uh oh");
		m_currentBlock->AddComponent<Rigidbody>();
		//OnStop();
	}
	m_state = GameState::Lost;
}

Vector3 APCore::Darken(const Vector3& OutColor, float percent)
{
	return Vector3(OutColor.X() - OutColor.X() * percent, OutColor.Y() - OutColor.Y() * percent, OutColor.Z() - OutColor.Z() * percent);
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
