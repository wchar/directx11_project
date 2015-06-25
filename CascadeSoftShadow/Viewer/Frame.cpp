#include "Frame.h"
#include "WE.h"
#include "D3DPanel.h"
#include "PropertiesPanel.h"

static D3DPanel* s_d3dPanel = nullptr;
CEFrame::CEFrame(wxFrame *frame, const wxString& title )
	: fbFrame(frame, -1, title, wxDefaultPosition, wxSize(1100,700))
{
	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	s_d3dPanel = s_d3dPanel = new D3DPanel(_d3dPanel);
	bSizer->Add(s_d3dPanel, 1, wxEXPAND, 5);
	_d3dPanel->SetSizer(bSizer);
	
	auto manager = s_d3dPanel->getRenderer()->getManager();
	bSizer = new wxBoxSizer(wxVERTICAL);
	auto cp = new CascadePane(_propertiesPanel, s_d3dPanel, manager);
	bSizer->Add(cp, 1, wxEXPAND, 5);
	_propertiesPanel->SetSizer(bSizer);

	_split->SetSashPosition(_split->GetSashPosition() + 1);

	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CEFrame::onClose));
}

CEFrame::~CEFrame()
{
	this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CEFrame::onClose));
}

static bool s_closed = false;
static DWORD WINAPI s_safe_update(LPVOID p)
{
	static auto s_finished = true;
	while (!s_closed)
	{
		if (s_finished)
		{
			s_finished = false;
			s_d3dPanel->tick([]()
			{
				s_finished = true;
			});
		}
		else
		{
			Sleep(60);
		}
		Sleep(30);
	}

	return 0;
}


bool CEFrame::create()
{
	DWORD threadId;
	s_closed = false;
	CreateThread(NULL, 0, s_safe_update, (LPVOID)0, 0, &threadId);

	return true;
}

void CEFrame::onClose(wxCloseEvent& event)
{
	s_closed = true;
	Sleep(300);
	event.Skip();
}