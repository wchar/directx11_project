#pragma once

#include "wxfb.h"
#include "WE.h"
#include "D3DPanel.h"

///////////////////////////////////////////////////////////////////////////////
/// Class CascadePane
///////////////////////////////////////////////////////////////////////////////
class CascadePane : public fbCascadePane
{
public:
	CascadePane(wxWindow* parent, D3DPanel* d3dPanel, WE::CascadeManager* pCascadeManager);
	~CascadePane();


protected:
	virtual void OnVCChecked(wxCommandEvent& event);
	virtual void OnTSChanged(wxScrollEvent& event);
	virtual void OnSBChanged(wxScrollEvent& event);
	virtual void OnCBChecked(wxCommandEvent& event);
	virtual void OnCBChanged(wxScrollEvent& event);
	virtual void OnSelectionChanged(wxCommandEvent& event);
	virtual void OnLevelCountChanged(wxCommandEvent& event);
	virtual void OnL1Changed(wxScrollEvent& event){ OnAnyLevelChanged(0); }
	virtual void OnL2Changed(wxScrollEvent& event){ OnAnyLevelChanged(1); }
	virtual void OnL3Changed(wxScrollEvent& event){ OnAnyLevelChanged(2); }
	virtual void OnL4Changed(wxScrollEvent& event){ OnAnyLevelChanged(3); }
	virtual void OnL5Changed(wxScrollEvent& event){ OnAnyLevelChanged(4); }
	virtual void OnL6Changed(wxScrollEvent& event){ OnAnyLevelChanged(5); }
	virtual void OnL7Changed(wxScrollEvent& event){ OnAnyLevelChanged(6); }
	virtual void OnL8Changed(wxScrollEvent& event){ OnAnyLevelChanged(7); }

	void OnAnyLevelChanged(int level);

	virtual void on_world_size_changed(wxCommandEvent& event);
	virtual void on_camera_far_changed(wxCommandEvent& event);
	virtual void on_show_cascade(wxCommandEvent& event);
	virtual void on_mesh_changed(wxCommandEvent& event);
	WE::CascadeManager* _cascadeManager;
	D3DPanel* _d3dPanel;
};