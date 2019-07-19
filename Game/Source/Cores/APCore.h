#pragma once
#include "ECS/Core.h"
#include "Components/Transform.h"
#include "ECS/Entity.h"
#include "Engine/World.h"
#include "ECS/Component.h"
#include "Components/Graphics/Model.h"
#include "Engine/Input.h"
#include "Math/Vector3.h"

class StackBlock final
	: public Component<StackBlock>
{
public:
	virtual void Serialize(json& outJson) override
	{
	}

	virtual void Deserialize(const json& inJson) override
	{
	}

	virtual void OnEditorInspect() override
	{
		ImGui::Checkbox("Movement Axis Z", &LeftSideBlock);
		ImGui::Checkbox("Block Direction", &BlockDirection);
		ImGui::DragFloat("Movement Speed", &BlockMoveSpeed);
	}

	virtual void Init() override
	{
	}

	float BlockMoveSpeed = 1.0f;

	bool LeftSideBlock = false;
	bool BlockDirection = true;

};
ME_REGISTER_COMPONENT(StackBlock)

class APCore final
	: public Core<APCore>
{
	const float kStartDistance = 5.0f;
public:
	APCore()
		: Base(ComponentFilter().Requires<Transform>().Requires<StackBlock>())
	{
	}
	virtual void OnEntityAdded(Entity& NewEntity) final
	{
	}


	virtual void OnEntityRemoved(Entity& InEntity) final
	{
	}


	virtual void OnEditorInspect() final
	{
		Base::OnEditorInspect();
	}


	virtual void Update(float dt) final
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

		if (Input::GetInstance().IsKeyDown(KeyCode::Space))
		{

		}
	}

private:
	virtual void Init() final
	{
	}

	virtual void OnStart() override
	{
		m_previousBlock = GetWorld().CreateEntity().lock();
		Transform& prevTransform = m_previousBlock->AddComponent<Transform>("Root StackBlock");
		Vector3 prevScale = prevTransform.GetScale();
		prevScale.SetX(2.f);
		prevScale.SetY(2.f);
		prevScale.SetZ(2.f);
		prevTransform.SetScale(prevScale);
		StackBlock& prevBlock = m_previousBlock->AddComponent<StackBlock>();
		prevBlock.BlockMoveSpeed = 0.f;
		m_previousBlock->AddComponent<Model>("Assets/Cube.fbx");

		m_currentBlock = GetWorld().CreateEntity().lock();
		Transform& transform = m_currentBlock->AddComponent<Transform>("Root StackBlock");
		transform.SetPosition(prevTransform.Position + Vector3(0.f, (prevTransform.GetScale().Y()) + (.3f), 0.f));
		Vector3 scale = transform.GetScale();
		scale.SetX(2.f);
		scale.SetY(.3f);
		scale.SetZ(2.f);
		transform.SetScale(scale);
		StackBlock& block = m_currentBlock->AddComponent<StackBlock>();
		m_currentBlock->AddComponent<Model>("Assets/Cube.fbx");
	}

	SharedPtr<Entity> m_currentBlock;
	SharedPtr<Entity> m_previousBlock;
	Vector3 m_currentPosition;
};
ME_REGISTER_CORE(APCore)