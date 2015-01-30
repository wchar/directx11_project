#pragma once

#include <wx/wx.h>

class CEFrame : public wxFrame
{
public:
	CEFrame(wxFrame *frame, const wxString& title);
	~CEFrame();

	bool create();
private:


};
