#include "GameUIView.h"
#include "Ultralight/View.h"

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
	ExecuteScript("document.getElementById('Score').innerHTML='" + ((Score == 0) ? "HEAP" : std::to_string(Score)) + "';");
	if (Score == 0)
	{
		ExecuteScript("document.getElementById('SpaceToPlay').style.display = 'block'; ");
	}
	else
	{
		ExecuteScript("document.getElementById('SpaceToPlay').style.display = 'none';");
	}
}

void GameUIView::SetMessage(const std::string& NewMessage)
{
	ExecuteScript("document.getElementById('Score').innerHTML='" + NewMessage + "';");
}

#if USING( ME_EDITOR )

void GameUIView::OnEditorInspect()
{

}
#endif
