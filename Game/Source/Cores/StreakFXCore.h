#pragma once
#include "ECS/Core.h"
#include "Components/Transform.h"

class StreakFX
	: public Component<StreakFX>
{
public:
	StreakFX()
		: Component("StreakFX")
	{

	}

	void Init() override
	{
	}

private:
	void OnSerialize(json& outJson) override
	{
	}

	void OnDeserialize(const json& inJson) override
	{
	}
};

ME_REGISTER_COMPONENT(StreakFX)

class StreakFXCore final
	: public Core<StreakFXCore>
{
public:
	StreakFXCore()
		: Base(ComponentFilter().Requires<StreakFX>().Requires<Transform>())
	{

	}

	~StreakFXCore()
	{
	}

	virtual void OnEntityAdded(Entity& NewEntity) override
	{

	}

	virtual void OnEntityRemoved(Entity& InEntity) override
	{

	}

#if USING( ME_EDITOR )
	virtual void OnEditorInspect() override
	{

	}
#endif

	virtual void Update(const UpdateContext& dt) override
	{
		auto& ents = GetEntities();
		for (auto& ent : ents)
		{
			Transform& trans = ent.GetComponent<Transform>();
			trans.SetScale(trans.GetScale() + (6.f * dt.GetDeltaTime()));
		}
	}

	virtual void Init() override
	{

	}

};

ME_REGISTER_CORE(StreakFXCore)