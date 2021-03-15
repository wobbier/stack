#pragma once
#include "ECS/Core.h"

#include "Components/Transform.h"
#include "ECS/Entity.h"
#include "ECS/EntityHandle.h"
#include "Engine/World.h"
#include "Engine/Input.h"
#include "Math/Random.h"
#include "Math/Vector3.h"
#include "Mathf.h"

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

	virtual void Init() final;
	virtual void OnStart() override;
	virtual void OnEntityAdded(Entity& NewEntity) final;
	virtual void OnEntityRemoved(Entity& InEntity) final;
	virtual void OnStop() override;

	virtual void Update(float dt) final;

#if ME_EDITOR
	virtual void OnEditorInspect() final;
#endif

private:

	Random64 random;
	float testPercent = 0.f;
	bool initialized = false;
	float blockPercent = 0.f;

	void GenerateNextHue();
	Vector3 GetHue(float percent);
	Vector3 Darken(const Vector3& OutColor, float percent);

	void SpawnNextBlock();
	void CreateBrokenPiece(float amountLost, Vector3 position, bool blockMovingOnX, bool PositiveDirection);

	void AddStreakFX(Vector3 pos, int streakNum);

	void SetupCamera();
	bool EndBlock();
	unsigned int UpdateScore();
	void Reset(Transform& transform);

	void LoseGame();
	void ClearBlocks();
	EntityHandle m_uiScore;
	EntityHandle m_currentBlock;
	EntityHandle m_previousBlock;
	EntityHandle m_mainCamera;
	Vector3 m_currentPosition;
	Vector3 m_currentStackSize;
	Vector2 m_gridSnapSize;
	float m_currentStreak = 0;

	float m_startTime = 0.f;
	float m_totalTime = 0.f;
	float m_cameraTravelDistance;
	float m_cameraFocusSpeed = 2.f;
	float m_cameraHeightOffset = 5.f;
	bool m_isKeyPressed = false;
	float m_fracJourney = 0.f;
	GameState m_state = GameState::Start;
};
ME_REGISTER_CORE(APCore)