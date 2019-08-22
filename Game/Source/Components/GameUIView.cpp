#include "GameUIView.h"
#include "Ultralight/View.h"
#include "UI/JSHelpers.h"

GameUIView::GameUIView()
	: BasicUIView("GameUIView")
{
	FilePath = Path("Assets/UI/GameUI.html");
}

void GameUIView::OnUILoad(ultralight::JSObject& GlobalWindow, ultralight::View* Caller)
{
	GlobalWindow["GetScore"] = BindJSCallback(&GameUIView::GetScore);
	UpdateScore(Score);
}

void GameUIView::GetScore(const ultralight::JSObject& thisObject, const ultralight::JSArgs& args)
{
	//UpdateScore(Score);
}

void GameUIView::UpdateScore(unsigned int NewScore)
{
	Score = NewScore;
	ultralight::JSEval(std::string("document.getElementById('Score').innerHTML='" + std::to_string(Score) + "';").c_str());
}

#if ME_EDITOR

void GameUIView::OnEditorInspect()
{

}
#endif