#include "GameUIView.h"
#include "Ultralight/View.h"

GameUIView::GameUIView()
	: BasicUIView("GameUIView")
{
	FilePath = Path("Assets/UI/GameUI.html");
}

#if USING( ME_UI )
void GameUIView::OnUILoad(ultralight::JSObject& GlobalWindow, ultralight::View* Caller)
{
	GlobalWindow["GetScore"] = BindJSCallback(&GameUIView::GetScore);
	UpdateScore(Score);
}

void GameUIView::GetScore(const ultralight::JSObject& thisObject, const ultralight::JSArgs& args)
{
	//UpdateScore(Score);
}
#endif

void GameUIView::UpdateScore(unsigned int NewScore)
{
	Score = NewScore;

#if USING( ME_UI )
	ExecuteScript("document.getElementById('Score').innerHTML='" + ((Score == 0) ? "HEAP" : std::to_string(Score)) + "';");
	if (Score == 0)
	{
		ExecuteScript("document.getElementById('SpaceToPlay').style.display = 'block'; ");
	}
	else
	{
		ExecuteScript("document.getElementById('SpaceToPlay').style.display = 'none';");
	}
#endif
}

void GameUIView::SetMessage(const std::string& NewMessage)
{
#if USING( ME_UI )
	ExecuteScript("document.getElementById('Score').innerHTML='" + NewMessage + "';");
#endif
}

#if USING( ME_EDITOR )

void GameUIView::OnEditorInspect()
{

}
#endif
