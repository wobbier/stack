#pragma once
#include "Components/UI/BasicUIView.h"
#include "UI/JSHelpers.h"

namespace ultralight { class View; }

class IntroUI
	: public BasicUIView
{
public:
	IntroUI();

	virtual void OnUILoad(ultralight::JSObject& GlobalWindow, ultralight::View* Caller) final;

#if ME_EDITOR
	virtual void OnEditorInspect() override;
#endif

private:
	int Score = 0;
	virtual void LoadGame(const ultralight::JSObject& thisObject, const ultralight::JSArgs& args) final;
};

ME_REGISTER_COMPONENT(IntroUI)
