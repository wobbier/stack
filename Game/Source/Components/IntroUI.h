#pragma once
#include "Components/UI/BasicUIView.h"

#if USING( ME_PLATFORM_WINDOWS )

#include "UI/JSHelpers.h"

namespace ultralight { class View; }

class IntroUI
	: public BasicUIView
{
public:
	IntroUI();

#if USING( ME_UI )

	virtual void OnUILoad(ultralight::JSObject& GlobalWindow, ultralight::View* Caller) final;
#endif

#if USING( ME_EDITOR ) 
	virtual void OnEditorInspect() override;
#endif

private:
    int Score = 0;
#if USING( ME_UI )
	virtual void LoadGame(const ultralight::JSObject& thisObject, const ultralight::JSArgs& args) final;
#endif
};

ME_REGISTER_COMPONENT(IntroUI)

#endif
