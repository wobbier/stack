#pragma once
#include "ECS/Component.h"
#include "ECS/Core.h"
#include "Components/Camera.h"

class CameraDolley
	: public Component<CameraDolley>
{
public:
	virtual void Serialize(json& outJson) override
	{
	}

	virtual void Deserialize(const json& inJson) override
	{
	}
#if ME_EDITOR
	virtual void OnEditorInspect() override
	{
	}
#endif
	virtual void Init() override
	{
	}
};
ME_REGISTER_COMPONENT(CameraDolley)

class CameraDolleyCore
	: public Core<CameraDolleyCore>
{
public:
	CameraDolleyCore()
		: Base(ComponentFilter().Requires<Camera>().Requires<CameraDolley>())
	{

	}
	virtual void OnEntityAdded(Entity& NewEntity) override
	{
	}

	virtual void OnEntityRemoved(Entity& InEntity) override
	{
	}

#if ME_EDITOR
	virtual void OnEditorInspect() override
	{
		Base::OnEditorInspect();
	}
#endif

	virtual void Update(float dt) override
	{
	}

private:
	virtual void Init() override
	{
	}
};
ME_REGISTER_CORE(CameraDolleyCore)
