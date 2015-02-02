#include "App.h"
#include "Frame.h"

// show console 
#pragma comment(linker,"/subsystem:console /entry:WinMainCRTStartup")

IMPLEMENT_APP(CEApp);

bool CEApp::OnInit()
{    	    
    auto frame = new CEFrame(nullptr, L"asd");
	
	if (frame->create())
	{
		frame->Show();
		return true;
	}	

	return false;
}
