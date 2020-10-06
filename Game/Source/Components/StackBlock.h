#pragma once
#include "ECS/ComponentDetail.h"
#include "ECS/Component.h"
#include "Math/Vector3.h"

class StackBlock final
	: public Component<StackBlock>
{
public:
	StackBlock();

	virtual void Init() final;

	virtual void OnSerialize(json& outJson) final;
	virtual void OnDeserialize(const json& inJson) final;

	float BlockMoveSpeed = 4.0f;

	bool MoveOnX = false;
	bool BlockDirection = false;

	Vector3 Color;

#if ME_EDITOR
	virtual void OnEditorInspect() final;
#endif
};
ME_REGISTER_COMPONENT(StackBlock)