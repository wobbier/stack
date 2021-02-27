#include "stdafx.h"
#include "StackGame.h"
#include "Engine/Engine.h"
#include "SDL.h"
#include "SDL_video.h"
//#include "App.h"
//
//using namespace Windows::ApplicationModel::Core;
//
//ref class GameApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
//{
//public:
//	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
//	{
//		return ref new App();
//	}
//};
//
//[Platform::MTAThread]
//int main(Platform::Array<Platform::String^>^)
//{
//	auto applicationSource = ref new GameApplicationSource();
//	CoreApplication::Run(applicationSource);
//	return 0;
//}

#include "SDL_main.h"
#include <wrl.h>

/* At least one file in any SDL/WinRT app appears to require compilation
   with C++/CX, otherwise a Windows Metadata file won't get created, and
   an APPX0702 build error can appear shortly after linking.

   The following set of preprocessor code forces this file to be compiled
   as C++/CX, which appears to cause Visual C++ 2012's build tools to
   create this .winmd file, and will help allow builds of SDL/WinRT apps
   to proceed without error.

   If other files in an app's project enable C++/CX compilation, then it might
   be possible for SDL_winrt_main_NonXAML.cpp to be compiled without /ZW,
   for Visual C++'s build tools to create a winmd file, and for the app to
   build without APPX0702 errors.  In this case, if
   SDL_WINRT_METADATA_FILE_AVAILABLE is defined as a C/C++ macro, then
   the #error (to force C++/CX compilation) will be disabled.

   Please note that /ZW can be specified on a file-by-file basis.  To do this,
   right click on the file in Visual C++, click Properties, then change the
   setting through the dialog that comes up.
*/
#ifndef SDL_WINRT_METADATA_FILE_AVAILABLE
#ifndef __cplusplus_winrt
#error SDL_winrt_main_NonXAML.cpp must be compiled with /ZW, otherwise build errors due to missing .winmd files can occur.
#endif
#endif

/* Prevent MSVC++ from warning about threading models when defining our
   custom WinMain.  The threading model will instead be set via a direct
   call to Windows::Foundation::Initialize (rather than via an attributed
   function).

   To note, this warning (C4447) does not seem to come up unless this file
   is compiled with C++/CX enabled (via the /ZW compiler flag).
*/
#ifdef _MSC_VER
#pragma warning(disable:4447)
#endif

/* Make sure the function to initialize the Windows Runtime gets linked in. */
#ifdef _MSC_VER
#pragma comment(lib, "runtimeobject.lib")
#endif

int _main(int args, char** thing)
{

	SDL_DisplayMode mode; SDL_Window* window = NULL; SDL_Renderer* renderer = NULL; SDL_Event evt;

	//if (SDL_Init(SDL_INIT_VIDEO) != 0) {
	//	return 1;
	//}

	//if (SDL_GetCurrentDisplayMode(0, &mode) != 0) {
	//	return 1;
	//}

	//if (SDL_CreateWindowFrom(mode.w, mode.h, SDL_WINDOW_FULLSCREEN, &window, &renderer) != 0) {
	//	return 1;
	//}

	//while (1) {
	//	while (SDL_PollEvent(&evt)) {
	//	}

	//	int x = 0;
	//	int y = 0;
	//	SDL_GetMouseState(&x, &y);
	//	SDL_GetGlobalMouseState(&x, &y);

	//	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	//	SDL_RenderClear(renderer);
	//	SDL_RenderPresent(renderer);
	//}

	StackGame* game = new StackGame();
	GetEngine().Init(game);
	GetEngine().Run();
	return 1;
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return SDL_WinRTRunApp(_main, NULL);
}
