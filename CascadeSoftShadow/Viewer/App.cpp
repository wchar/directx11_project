#include "App.h"
#include "Frame.h"

IMPLEMENT_APP(CEApp);

bool CEApp::OnInit()
{    
    wxApp::OnInit();

    auto frame = new CEFrame(nullptr, L"asd");
	
	if (frame->create())
	{
		frame->Show();
		return true;
	}
	
	return false;
}
