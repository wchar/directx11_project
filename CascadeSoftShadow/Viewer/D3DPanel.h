#pragma once

#include <functional>
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

	void tick(function<void()> callback);

	void setMesh(int i);
private:
	void createD3DWin();
	void onSize(wxSizeEvent& evt);
	void onMouseEvent(wxMouseEvent& evt);

	WE::ModelViewerCamera* _camera;
	WE::Renderer* _renderer;
	WE::Mesh* _meshs[4];
	DECLARE_EVENT_TABLE();
};

