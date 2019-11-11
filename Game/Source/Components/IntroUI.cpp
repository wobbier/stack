#include "IntroUI.h"
#include "Ultralight/View.h"
#include "UI/JSHelpers.h"
#include "HavanaEvents.h"

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