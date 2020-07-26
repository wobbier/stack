#include "APCore.h"
#include "Components/Physics/Rigidbody.h"
#include "Graphics/Texture.h"
#include "Resource/ResourceCache.h"
#include "Components/GameUIView.h"
#include "Components/Audio/AudioSource.h"
#include "Mathf.h"
#include "Cores/Utility/SelfDestructCore.h"

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
	if (m_currentBlock)
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
	if (!m_isKeyPressed && (Keyboard.Space || Controller.buttons.a))// || testColors <= 40)
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

	m_currentBlock = GetWorld().CreateEntity();
	Transform& prevTransform = m_currentBlock->AddComponent<Transform>("Root StackBlock");
	prevTransform.SetScale(Vector3(2.f));
	StackBlock& prevBlock = m_currentBlock->AddComponent<StackBlock>();
	prevBlock.BlockMoveSpeed = 0.f;
	m_currentBlock->AddComponent<Model>("Assets/Cube.fbx");

	prevBlock.Color = (m_colors[m_currentColorIndex]);
	for (SharedPtr<Transform> child : prevTransform.GetChildren())
	{
		child->Reset();
		if (child->Parent->HasComponent<Mesh>())
		{
			m_currentColorStepPercent += m_colorStepPercent;
			Mesh& mesh = child->Parent->GetComponent<Mesh>();
			mesh.MeshMaterial->DiffuseColor = Vector3::Lerp(m_colors[m_currentColorIndex], m_colors[m_targetColorIndex], m_currentColorStepPercent);
			mesh.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/CubeDiffuse.jpg")));
			mesh.MeshMaterial->SetTexture(Moonlight::TextureType::Opacity, ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/BaseCubeDiffuse.jpg")));
		}
	}
	SpawnNextBlock();
	StackBlock& curentStackBlock = m_currentBlock->AddComponent<StackBlock>();
	Camera::CurrentCamera->ClearColor = curentStackBlock.Color;

	m_currentBlock->SetActive(false);
	SetupCamera();
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

			block.Color = Vector3::Lerp(m_colors[m_currentColorIndex], m_colors[m_targetColorIndex], m_currentColorStepPercent);
			mesh.MeshMaterial->DiffuseColor = block.Color;
			BRUH(std::to_string(m_currentColorStepPercent) + ": " + block.Color.ToString());
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
			BRUH("New Color(" + std::to_string(m_targetColorIndex) + "): " + m_colors[m_targetColorIndex].ToString() + " - Old: " + m_colors[GetPreviousColorIndex()].ToString());
			Camera::CurrentCamera->ClearColor = prevBlock.Color;
		}
	}

	{
		// Keep a note of the time the movement started.
		m_startTime = 0.f;

		// Calculate the journey length.
		m_cameraTravelDistance = (prevTransform.GetPosition() - transform.GetPosition()).Length();

		m_totalTime = 0.f;
	}
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

	cameraEnt->SetPosition(cameraEnt->GetPosition() + Vector3(0.f, 5.f, 0.f));
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
			child->Parent->AddComponent<SelfDestruct>(5.f);
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
	BRUH("You Lost!");
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

		view.SetMessage("You Fucked Up");
		OnStop();
	}
	m_state = GameState::Lost;
}
