#pragma once
#include "Components/UI/BasicUIView.h"

namespace ultralight { class View; }

class GameUIView
	: public BasicUIView
{
public:
	GameUIView();

#if USING( ME_UI )
	virtual void OnUILoad(ultralight::JSObject& GlobalWindow, ultralight::View* Caller) final;
#endif

	void UpdateScore(unsigned int NewScore);
	void SetMessage(const std::string& NewMessage);

#if USING( ME_EDITOR )
	virtual void OnEditorInspect() override;
#endif

private:
    int Score = 0;
#if USING( ME_UI )
	virtual void GetScore(const ultralight::JSObject& thisObject, const ultralight::JSArgs& args) final;
#endif
};

ME_REGISTER_COMPONENT(GameUIView)
