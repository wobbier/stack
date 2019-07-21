#include "APCore.h"

APCore::APCore()
	: Base(ComponentFilter().Requires<Transform>().Requires<StackBlock>())
	, GridSnapSize(5.f, 5.f)
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
	ImGui::DragFloat("Camera Height Offset", &CameraHeightOffset);
	ImGui::DragFloat2("Grid Size", &GridSnapSize.GetInternalVec()[0]);
}

void APCore::Update(float dt)
{
	Transform& transform = m_currentBlock->GetComponent<Transform>();
	StackBlock& block = m_currentBlock->GetComponent<StackBlock>();
	m_currentPosition = transform.GetPosition();
	if (block.LeftSideBlock)
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

	totalTime += dt;
	float distCovered = (totalTime - m_startTime) * FocusSpeed;
	float fracJourney = distCovered / TravelDistance;

	Transform& camTransform = m_mainCamera->GetComponent<Transform>();
	if (fracJourney < 1.0f)
	{
		camTransform.SetPosition(Mathf::Lerp(camTransform.GetPosition(), Vector3(camTransform.Position.X(), m_currentPosition.Y() + CameraHeightOffset, camTransform.Position.Z()), fracJourney));
	}

	if (!KeyPressed && Input::GetInstance().IsKeyDown(KeyCode::Space))
	{
		EndBlock();
		SpawnNextBlock();
		KeyPressed = false;
	}
	KeyPressed = Input::GetInstance().IsKeyDown(KeyCode::Space);
}

void APCore::Init()
{

}

void APCore::OnStart()
{
	m_currentBlock = GetWorld().CreateEntity().lock();
	Transform& prevTransform = m_currentBlock->AddComponent<Transform>("Root StackBlock");
	Vector3 prevScale = prevTransform.GetScale();
	prevScale.SetX(2.f);
	prevScale.SetY(2.f);
	prevScale.SetZ(2.f);
	prevTransform.SetScale(prevScale);
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

	Vector3 scale = transform.GetScale();
	scale.SetX(prevScale.X());
	scale.SetY(.3f);
	scale.SetZ(prevScale.Z());
	transform.SetScale(scale);

	block.LeftSideBlock = !prevBlock.LeftSideBlock;
	if (block.LeftSideBlock)
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
		TravelDistance = (prevTransform.GetPosition() - transform.GetPosition()).Length();

		totalTime = 0.f;
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
	pos.SetX(Mathf::Round(pos.X() * GridSnapSize.X()) / GridSnapSize.X());
	pos.SetZ(Mathf::Round(pos.Z() * GridSnapSize.Y()) / GridSnapSize.Y());

	if (!block.LeftSideBlock)
	{
		float distanceFailedX = pos.X() - prevPos.X();
		if (distanceFailedX != 0.f)
		{
			float distAbs = Mathf::Abs(distanceFailedX);
			Vector3 scale = transform.GetScale();
			float scaleThing = prevTransform.GetScale().X() - (distAbs / 2.f);
			scale.SetX(scaleThing);
			pos.SetX(pos.X() - (distanceFailedX / 2.f));
			if (scale.X() < 0.f)
			{
				scale.SetX(2.f);
			}
			transform.SetScale(scale);
		}
	}
	else
	{

		float distanceFailedZ = pos.Z() - prevPos.Z();
		if (distanceFailedZ != 0.f)
		{
			float distAbs = Mathf::Abs(distanceFailedZ);
			Vector3 scale = transform.GetScale();
			float scaleThing = prevTransform.GetScale().Z() - (distAbs / 2.f);
			scale.SetZ(scaleThing);
			pos.SetZ(pos.Z() - (distanceFailedZ / 2.f));
			if (scale.Z() < 0.f)
			{
				scale.SetZ(2.f);
			}
			transform.SetScale(scale);
		}
	}

	transform.SetPosition(pos);
}
