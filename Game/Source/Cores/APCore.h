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
		ImGui::Checkbox("Movement Axis Z", &MoveOnX);
		ImGui::Checkbox("Block Direction", &BlockDirection);
		ImGui::DragFloat("Movement Speed", &BlockMoveSpeed);
	}

	virtual void Init() override
	{
	}

	float BlockMoveSpeed = 2.0f;

	bool MoveOnX = false;
	bool BlockDirection = false;

	Vector3 Color;

};
ME_REGISTER_COMPONENT(StackBlock)

class APCore final
	: public Core<APCore>
{
	const float kErrorMargin = 0.1f;
	const float kStartDistance = 5.0f;
	const float kBoundsLimit = 4.f;
public:
	APCore();

	virtual void OnEntityAdded(Entity& NewEntity) final;
	virtual void OnEntityRemoved(Entity& InEntity) final;
	virtual void OnEditorInspect() final;

	virtual void Update(float dt) final;

private:
	virtual void Init() final;
	virtual void OnStart() override;
	virtual void OnStop() override;

	void SpawnNextBlock();
	void CreateBrokenPiece(float amountLost, Vector3 something);
	void SetupCamera();
	void EndBlock();

	void Reset(Transform& transform);

	SharedPtr<Entity> m_currentBlock;
	SharedPtr<Entity> m_previousBlock;
	SharedPtr<Entity> m_mainCamera;
	Vector3 m_currentPosition;
	Vector3 m_currentStackSize;
	Vector2 m_gridSnapSize;
	Vector3 m_colorStep;
	float m_startTime = 0.f;
	float m_totalTime = 0.f;
	float m_cameraTravelDistance;
	float m_cameraFocusSpeed = 2.f;
	float m_cameraHeightOffset = 5.f;
	bool m_isKeyPressed = false;
	float m_fracJourney = 0.f;
};
ME_REGISTER_CORE(APCore)