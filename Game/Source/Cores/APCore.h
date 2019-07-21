#pragma once
#include "ECS/Core.h"
#include "Components/Transform.h"
#include "ECS/Entity.h"
#include "Engine/World.h"
#include "ECS/Component.h"
#include "Components/Graphics/Model.h"
#include "Engine/Input.h"
#include "Math/Vector3.h"
#include "Components/Graphics/Mesh.h"
#include "Engine/Engine.h"
#include "Cores/SceneGraph.h"
#include "Components/Camera.h"
#include "Mathf.h"

class StackBlock final
	: public Component<StackBlock>
{
public:
	StackBlock()
		: Color(1.f)
	{
	}

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

	float BlockMoveSpeed = 2.0f;

	bool LeftSideBlock = false;
	bool BlockDirection = false;

	Vector3 Color;

};
ME_REGISTER_COMPONENT(StackBlock)

class APCore final
	: public Core<APCore>
{
	const float kStartDistance = 5.0f;
public:
	APCore();

	virtual void OnEntityAdded(Entity& NewEntity) final;
	virtual void OnEntityRemoved(Entity& InEntity) final;
	virtual void OnEditorInspect() final;

	virtual void Update(float dt) final;

private:
	virtual void Init() final;
	virtual void OnStart() override;

	void SpawnNextBlock();
	void SetupCamera();
	void EndBlock();

	SharedPtr<Entity> m_currentBlock;
	SharedPtr<Entity> m_previousBlock;
	SharedPtr<Entity> m_mainCamera;
	Vector3 m_currentPosition;
	Vector2 GridSnapSize;
	float m_startTime = 0.f;
	float totalTime;
	float TravelDistance;
	float FocusSpeed = 2.f;
	float CameraHeightOffset = 5.f;
	bool KeyPressed = false;
};
ME_REGISTER_CORE(APCore)