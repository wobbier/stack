#include "IntroUI.h"
#include "Ultralight/View.h"

#if ME_PLATFORM_UWP || ME_PLATFORM_WIN64

#include "UI/JSHelpers.h"
#include "Events/SceneEvents.h"

IntroUI::IntroUI()
	: BasicUIView("IntroUI")
{
	FilePath = Path("Assets/UI/IntroUI.html");
}

void IntroUI::OnUILoad(ultralight::JSObject& GlobalWindow, ultralight::View* Caller)
{
	GlobalWindow["LoadGame"] = BindJSCallback(&IntroUI::LoadGame);
}

void IntroUI::LoadGame(const ultralight::JSObject& thisObject, const ultralight::JSArgs& args)
{
	LoadSceneEvent evt;
	//evt.Level = 
	ultralight::String thing = args[0].ToString();
	std::string news = std::string(thing.utf8().data());
	evt.Level = news;
	evt.Fire();
}

#if ME_EDITOR

void IntroUI::OnEditorInspect()
{

}

#endif

#endif
