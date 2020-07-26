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
	ultralight::JSEval(std::string("document.getElementById('Score').innerHTML='" + ((Score == 0) ? "HEAP" : std::to_string(Score)) + "';").c_str());
	if (Score == 0)
	{
		ultralight::JSEval(std::string("document.getElementById('SpaceToPlay').innerHTML='Space to Play';").c_str());
	}
	else
	{
		ultralight::JSEval(std::string("document.getElementById('SpaceToPlay').innerHTML='';").c_str());
	}
}

void GameUIView::SetMessage(const std::string& NewMessage)
{
	ultralight::JSEval(std::string("document.getElementById('Score').innerHTML='" + NewMessage + "';").c_str());
}

#if ME_EDITOR

void GameUIView::OnEditorInspect()
{

}
#endif