#include "APCore.h"
#include "Components/Physics/Rigidbody.h"

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

void APCore::OnEditorInspect()
{
	Base::OnEditorInspect();
	ImGui::DragFloat("Camera Height Offset", &m_cameraHeightOffset);
	ImGui::DragFloat2("Grid Size", &m_gridSnapSize.GetInternalVec()[0]);
}

void APCore::Update(float dt)
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

	m_totalTime += dt;
	float distCovered = (m_totalTime - m_startTime) * m_cameraFocusSpeed;
	m_fracJourney = distCovered / m_cameraTravelDistance;

	Transform& camTransform = m_mainCamera->GetComponent<Transform>();
	if (m_fracJourney < 1.0f)
	{
		camTransform.SetPosition(Mathf::Lerp(camTransform.GetPosition(), Vector3(camTransform.Position.X(), m_currentPosition.Y() + m_cameraHeightOffset, camTransform.Position.Z()), m_fracJourney));
	}

	if (!m_isKeyPressed && Input::GetInstance().IsKeyDown(KeyCode::Space))
	{
		EndBlock();
		SpawnNextBlock();
		m_isKeyPressed = false;
	}
	m_isKeyPressed = Input::GetInstance().IsKeyDown(KeyCode::Space);
}

void APCore::Init()
{

}

void APCore::OnStart()
{
	m_currentBlock = GetWorld().CreateEntity().lock();
	Transform& prevTransform = m_currentBlock->AddComponent<Transform>("Root StackBlock");
	prevTransform.SetScale(m_currentStackSize);
	StackBlock& prevBlock = m_currentBlock->AddComponent<StackBlock>();
	prevBlock.BlockMoveSpeed = 0.f;
	m_currentBlock->AddComponent<Model>("Assets/Cube.fbx");
	for (Transform* child : prevTransform.GetChildren())
	{
		child->Reset();
	}
	SpawnNextBlock();

	SetupCamera();
}

void APCore::SpawnNextBlock()
{
	m_previousBlock = m_currentBlock;
	Transform& prevTransform = m_previousBlock->GetComponent<Transform>();
	StackBlock& prevBlock = m_previousBlock->GetComponent<StackBlock>();
	Vector3 prevScale = prevTransform.GetScale();

	m_currentBlock = GetWorld().CreateEntity().lock();
	Transform& transform = m_currentBlock->AddComponent<Transform>(std::string("StackBlock " + std::to_string(GetEntities().size())));
	StackBlock& block = m_currentBlock->AddComponent<StackBlock>();
	Model& model = m_currentBlock->AddComponent<Model>("Assets/Cube.fbx");

	m_currentStackSize.SetY(.3f);
	transform.SetScale(m_currentStackSize);

	block.MoveOnX = !prevBlock.MoveOnX;
	if (!block.MoveOnX)
	{
		transform.SetPosition(prevTransform.Position + Vector3(0.f, (prevTransform.GetScale().Y()) + (.3f), -kStartDistance));
		block.BlockDirection = !block.BlockDirection;
	}
	else
	{
		transform.SetPosition(prevTransform.Position + Vector3(kStartDistance, (prevTransform.GetScale().Y()) + (.3f), 0.f));
	}
	for (Transform* child : transform.GetChildren())
	{
		child->Reset();
		WeakPtr<Entity> ent = GetWorld().GetEntity(child->Parent);
		if (ent.lock()->HasComponent<Mesh>())
		{
			Mesh& mesh = ent.lock()->GetComponent<Mesh>();
			mesh.MeshMaterial->DiffuseColor.SetX(prevBlock.Color.X() - 0.1f);
			block.Color = mesh.MeshMaterial->DiffuseColor;
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
	Transform* cameraEnt = GetEngine().SceneNodes->RootTransform->GetChildByName("Main Camera");
	m_mainCamera = GetWorld().GetEntity(cameraEnt->Parent).lock();

	cameraEnt->SetPosition(Vector3(-5.f, m_currentBlock->GetComponent<Transform>().GetPosition().Y() + 5, 5.f));
	cameraEnt->SetRotation(Vector3(0.f, 45.f, 0.f));
	Camera& cam = m_mainCamera->GetComponent<Camera>();
	cam.Projection = Moonlight::ProjectionType::Orthographic;
	cam.UpdateCameraTransform(cameraEnt->GetPosition());
	cam.LookAt(Vector3(0.f, 0.f, 0.f));
}

void APCore::EndBlock()
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
			m_currentStackSize.SetX(scaleThing);

			if (m_currentStackSize.X() <= 0.f)
			{
				m_currentStackSize.SetX(2.f);
				m_currentStackSize.SetZ(2.f);
				transform.SetScale(m_currentStackSize);
				BRUH("You Lost!");
				return;
			}
			transform.SetScale(m_currentStackSize);

			float middle = prevTransform.GetPosition().X() + (transform.GetPosition().X() / 2.f);

			pos.SetZ(prevTransform.GetPosition().Z());

			pos.SetX((transform.GetPosition().X() + (scaleThing / 2.f)));
			CreateBrokenPiece((distAbs / 2.f), pos);
			pos.SetX(middle - (prevTransform.GetPosition().X() / 2.f));
		}
		else
		{
			if (m_currentStackSize.X() > kBoundsLimit)
			{
				m_currentStackSize.SetX(2.f);
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
			m_currentStackSize.SetZ(scaleThing);

			if (m_currentStackSize.Z() <= 0.f)
			{
				m_currentStackSize.SetX(2.f);
				m_currentStackSize.SetZ(2.f);
				transform.SetScale(m_currentStackSize);
				BRUH("You Lost!");
				return;
			}
			transform.SetScale(m_currentStackSize);



			float middle = prevTransform.GetPosition().Z() + (transform.GetPosition().Z() / 2.f);

			pos.SetX(prevTransform.GetPosition().X());
			pos.SetZ((transform.GetPosition().Z() + (scaleThing / 2.f)));
			CreateBrokenPiece((distAbs / 2.f), pos);

			pos.SetZ(middle - (prevTransform.GetPosition().Z() / 2.f));
		}
		else
		{
			if (m_currentStackSize.Z() > kBoundsLimit)
			{
				m_currentStackSize.SetZ(2.f);
			}
			pos.SetX(prevPos.X());
			pos.SetZ(prevPos.Z());
		}
	}

	transform.SetPosition(pos);
}

void APCore::CreateBrokenPiece(float amountLost, Vector3 something)
{
	StackBlock block = m_currentBlock->GetComponent<StackBlock>();

	SharedPtr<Entity> broken = GetWorld().CreateEntity().lock();
	Transform& transform = broken->AddComponent<Transform>("Broken Piece " + std::to_string(GetEntities().size()));
	Vector3 stack = m_currentStackSize;
	if (block.MoveOnX)
	{
		stack.SetX(amountLost);
	}
	else
	{
		stack.SetZ(amountLost);
	}
	transform.SetPosition(something);
	transform.SetScale(stack);
	broken->AddComponent<Model>("Assets/Cube.fbx");
	Rigidbody& rigidbody = broken->AddComponent<Rigidbody>(Rigidbody::ColliderType::Box);
	rigidbody.SetScale(stack);
	for (Transform* child : transform.GetChildren())
	{
		child->Reset();
		WeakPtr<Entity> ent = GetWorld().GetEntity(child->Parent);
		if (ent.lock()->HasComponent<Mesh>())
		{
			Mesh& mesh = ent.lock()->GetComponent<Mesh>();
			mesh.MeshMaterial->DiffuseColor.SetX(block.Color.X());
			block.Color = mesh.MeshMaterial->DiffuseColor;
		}
	}
}
