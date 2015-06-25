#pragma once

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/splitter.h>
#include <wx/sizer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame1
///////////////////////////////////////////////////////////////////////////////
class fbFrame : public wxFrame
{
private:

protected:
	wxPanel* _propertiesPanel;
	wxPanel* _d3dPanel;
	wxSplitterWindow* _split;
public:	
	fbFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(667, 434), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	~fbFrame();
};


///////////////////////////////////////////////////////////////////////////////
/// Class fbCascadePane
///////////////////////////////////////////////////////////////////////////////
class fbCascadePane : public wxPanel
{
private:

protected:
	wxCheckBox* m_checkBoxVC;
	wxStaticText* m_staticTextTS;
	wxSlider* m_sliderTS;
	wxStaticText* m_staticTextSB;
	wxSlider* m_sliderSB;
	wxCheckBox* m_checkBoxCB;
	wxStaticText* m_staticTextCB;
	wxSlider* m_sliderCB;
	wxComboBox* m_comboBoxSelection;
	wxComboBox* m_comboBoxLevelCount;
	wxSlider* m_sliderL1;
	wxSlider* m_sliderL2;
	wxSlider* m_sliderL3;
	wxSlider* m_sliderL4;
	wxSlider* m_sliderL5;
	wxSlider* m_sliderL6;
	wxSlider* m_sliderL7;
	wxSlider* m_sliderL8;

	// Virtual event handlers, overide them in your derived class
	virtual void OnVCChecked(wxCommandEvent& event) { event.Skip(); }
	virtual void OnTSChanged(wxScrollEvent& event) { event.Skip(); }
	virtual void OnSBChanged(wxScrollEvent& event) { event.Skip(); }
	virtual void OnCBChecked(wxCommandEvent& event) { event.Skip(); }
	virtual void OnCBChanged(wxScrollEvent& event) { event.Skip(); }
	virtual void OnSelectionChanged(wxCommandEvent& event) { event.Skip(); }
	virtual void OnLevelCountChanged(wxCommandEvent& event) { event.Skip(); }
	virtual void OnL1Changed(wxScrollEvent& event) { event.Skip(); }
	virtual void OnL2Changed(wxScrollEvent& event) { event.Skip(); }
	virtual void OnL3Changed(wxScrollEvent& event) { event.Skip(); }
	virtual void OnL4Changed(wxScrollEvent& event) { event.Skip(); }
	virtual void OnL5Changed(wxScrollEvent& event) { event.Skip(); }
	virtual void OnL6Changed(wxScrollEvent& event) { event.Skip(); }
	virtual void OnL7Changed(wxScrollEvent& event) { event.Skip(); }
	virtual void OnL8Changed(wxScrollEvent& event) { event.Skip(); }

	wxTextCtrl* _uiWorldSize;
	wxTextCtrl* _uiCameraFar;
	wxCheckBox* _uiShowCascade;
	wxComboBox* _uiMesh;
	// Virtual event handlers, overide them in your derived class
	virtual void on_world_size_changed(wxCommandEvent& event) { event.Skip(); }
	virtual void on_camera_far_changed(wxCommandEvent& event) { event.Skip(); }
	virtual void on_show_cascade(wxCommandEvent& event) { event.Skip(); }
	virtual void on_mesh_changed(wxCommandEvent& event) { event.Skip(); }
public:

	fbCascadePane(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(251, 636), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	~fbCascadePane();

};
