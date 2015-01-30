#include "Frame.h"
#include "WE.h"
#include "D3DPanel.h"

CEFrame::CEFrame(wxFrame *frame, const wxString& title )
	: wxFrame(frame, -1, title, wxDefaultPosition, wxSize(1100,700))
{
	D3DPanel* d3dPanel = new D3DPanel(this);
}

CEFrame::~CEFrame()
{

}


bool CEFrame::create()
{
	return true;
}