#pragma once
#include "Components/UI/BasicUIView.h"
#include "AppCore/JSHelpers.h"

namespace ultralight { class View; }

class GameUIView
	: public BasicUIView
{
public:
	GameUIView();

	virtual void OnUILoad(ultralight::JSObject& GlobalWindow, ultralight::View* Caller) final;

	void UpdateScore(unsigned int NewScore);

#if ME_EDITOR
	virtual void OnEditorInspect() override;
#endif

private:
	int Score = 0;
	virtual void GetScore(const ultralight::JSObject& thisObject, const ultralight::JSArgs& args) final;
};

ME_REGISTER_COMPONENT(GameUIView)
