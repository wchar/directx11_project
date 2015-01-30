#pragma once

#include <wx/wx.h>
#include "WE.h"

class D3DPanel : public wxPanel
{
public:
	D3DPanel(wxWindow*);
	~D3DPanel();

	WE::Renderer* getRenderer()
	{
		return _renderer;
	}
private:
	void createD3DWin();
	void onSize(wxSizeEvent& evt);
	void onMouseEvent(wxMouseEvent& evt);

	WE::ModelViewerCamera* _camera;
	WE::Renderer* _renderer;

	wxTimer _timer;
	void tick(wxTimerEvent& event);

	DECLARE_EVENT_TABLE();
};

