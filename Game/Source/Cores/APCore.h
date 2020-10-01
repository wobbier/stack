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
#include "Math/Random.h"
#include "ECS/EntityHandle.h"
#include "Math/Color.h"

class StackBlock final
	: public Component<StackBlock>
{
public:
	StackBlock()
		: Component("StackBlock")
		, Color(1.f)
	{
	}

	virtual void OnSerialize(json& outJson) final
	{
	}

	virtual void OnDeserialize(const json& inJson) final
	{
	}

#if ME_EDITOR
	virtual void OnEditorInspect() final
	{
		ImGui::Checkbox("Movement Axis Z", &MoveOnX);
		ImGui::Checkbox("Block Direction", &BlockDirection);
		ImGui::DragFloat("Movement Speed", &BlockMoveSpeed);
	}
#endif
	virtual void Init() final
	{
	}

	float BlockMoveSpeed = 4.0f;

	bool MoveOnX = false;
	bool BlockDirection = false;

	Vector3 Color;

};
ME_REGISTER_COMPONENT(StackBlock)

class APCore final
	: public Core<APCore>
{
	enum class GameState
	{
		Start = 0,
		Active,
		Lost,
		Restart
	};

	const float kErrorMargin = 0.1f;
	const float kStartDistance = 5.0f;
	const float kBoundsLimit = 4.f;
public:
	APCore();

	virtual void OnEntityAdded(Entity& NewEntity) final;
	virtual void OnEntityRemoved(Entity& InEntity) final;
#if ME_EDITOR
	virtual void OnEditorInspect() final;
#endif

	virtual void Update(float dt) final;

private:
	virtual void Init() final;
	virtual void OnStart() override;
	virtual void OnStop() override;

	Vector3 TestTargetColor;
	Vector3 TestDarkenedColor;

	Random64 random;
	float testPercent = 0.f;
	bool initialized = false;
	float blockPercent = 0.f;

	Vector3 GetHue(float percent);

	Vector3 GetNextColor();
	Vector3 Darken(const Vector3& OutColor, float percent);

	void SpawnNextBlock();

	void GenerateNextHue();

	void CreateBrokenPiece(float amountLost, Vector3 position);
	void SetupCamera();
	bool EndBlock();
	unsigned int UpdateScore();
	void Reset(Transform& transform);

	int GetPreviousColorIndex();
	void LoseGame();
	EntityHandle m_uiScore;
	EntityHandle m_currentBlock;
	EntityHandle m_previousBlock;
	EntityHandle m_mainCamera;
	Vector3 m_currentPosition;
	Vector3 m_currentStackSize;
	Vector2 m_gridSnapSize;


	float m_colorStepPercent = 0.1f;
	float m_currentColorStepPercent = 0.0f;
	int m_currentColorIndex = 0;
	int m_targetColorIndex = 1;
	std::vector<Vector3> m_colors;
	float m_startTime = 0.f;
	float m_totalTime = 0.f;
	float m_cameraTravelDistance;
	float m_cameraFocusSpeed = 2.f;
	float m_cameraHeightOffset = 10.f;
	bool m_isKeyPressed = false;
	float m_fracJourney = 0.f;
	Random64 m_random;
	GameState m_state = GameState::Start;
	std::queue<Vector3> m_colorQueue;
};
ME_REGISTER_CORE(APCore)