#pragma once

#include <wx/wx.h>
#include "wxfb.h"

class CEFrame : public fbFrame
{
public:
	CEFrame(wxFrame *frame, const wxString& title);
	~CEFrame();

	bool create();
private:
	void onClose(wxCloseEvent& event);
};
