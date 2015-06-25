#include "wxfb.h"

///////////////////////////////////////////////////////////////////////////

fbFrame::fbFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	_split = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE);

	_propertiesPanel = new wxPanel(_split, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	_propertiesPanel->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));

	_d3dPanel = new wxPanel(_split, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	_d3dPanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));

	_split->SplitVertically(_propertiesPanel, _d3dPanel, 300);
	bSizer1->Add(_split, 1, wxEXPAND, 5);

	this->SetSizer(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);
}

fbFrame::~fbFrame()
{
}




///////////////////////////////////////////////////////////////////////////
fbCascadePane::fbCascadePane(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer(wxVERTICAL);

	wxStaticText* m_staticText38;
	m_staticText38 = new wxStaticText(this, wxID_ANY, wxT("CascadeModifier"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	m_staticText38->Wrap(-1);
	m_staticText38->SetFont(wxFont(14, 74, 90, 90, false, wxT("Arial")));
	m_staticText38->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	m_staticText38->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTIONTEXT));
	m_staticText38->Hide();

	bSizer39->Add(m_staticText38, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Cascade Options")), wxVERTICAL);

	m_checkBoxVC = new wxCheckBox(this, wxID_ANY, wxT("Visualize Cascades"), wxDefaultPosition, wxDefaultSize, 0);
	m_checkBoxVC->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	sbSizer2->Add(m_checkBoxVC, 0, wxTOP | wxRIGHT | wxLEFT, 5);


	sbSizer2->Add(0, 15, 0, 0, 5);

	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextTS1;
	m_staticTextTS1 = new wxStaticText(this, wxID_ANY, wxT("Texture Size : "), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextTS1->Wrap(-1);
	m_staticTextTS1->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40->Add(m_staticTextTS1, 0, wxLEFT, 5);

	m_staticTextTS = new wxStaticText(this, wxID_ANY, wxT("512"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextTS->Wrap(-1);
	m_staticTextTS->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40->Add(m_staticTextTS, 0, wxLEFT, 5);


	sbSizer2->Add(bSizer40, 0, wxEXPAND, 5);

	m_sliderTS = new wxSlider(this, wxID_ANY, 512, 128, 2048, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
	sbSizer2->Add(m_sliderTS, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer401;
	bSizer401 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextSB2;
	m_staticTextSB2 = new wxStaticText(this, wxID_ANY, wxT("Shadow Blur :  "), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB2->Wrap(-1);
	m_staticTextSB2->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer401->Add(m_staticTextSB2, 0, wxLEFT, 5);

	m_staticTextSB = new wxStaticText(this, wxID_ANY, wxT("15"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB->Wrap(-1);
	m_staticTextSB->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer401->Add(m_staticTextSB, 0, wxLEFT, 5);


	sbSizer2->Add(bSizer401, 0, wxEXPAND, 5);

	m_sliderSB = new wxSlider(this, wxID_ANY, 1, 7, 66, wxDefaultPosition, wxDefaultSize, 0);
	sbSizer2->Add(m_sliderSB, 0, wxEXPAND, 5);


	sbSizer2->Add(0, 10, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer4011;
	bSizer4011 = new wxBoxSizer(wxHORIZONTAL);

	m_checkBoxCB = new wxCheckBox(this, wxID_ANY, wxT("Cascade Blur :"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer4011->Add(m_checkBoxCB, 0, wxALL, 5);

	m_staticTextCB = new wxStaticText(this, wxID_ANY, wxT("0.005"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextCB->Wrap(-1);
	m_staticTextCB->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer4011->Add(m_staticTextCB, 0, wxTOP | wxBOTTOM | wxLEFT, 5);


	sbSizer2->Add(bSizer4011, 0, wxEXPAND, 5);

	m_sliderCB = new wxSlider(this, wxID_ANY, 1, 0, 2000, wxDefaultPosition, wxDefaultSize, 0);
	sbSizer2->Add(m_sliderCB, 1, wxEXPAND, 5);


	sbSizer2->Add(0, 15, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer49;
	bSizer49 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticText46;
	m_staticText46 = new wxStaticText(this, wxID_ANY, wxT("Selection:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText46->Wrap(-1);
	bSizer49->Add(m_staticText46, 0, wxALL, 5);

	m_comboBoxSelection = new wxComboBox(this, wxID_ANY, wxT("Map Selection"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_comboBoxSelection->Append(wxT("Map Selection"));
	m_comboBoxSelection->Append(wxT("Interval Selection"));
	m_comboBoxSelection->Append(wxEmptyString);
	m_comboBoxSelection->SetSelection(0);
	bSizer49->Add(m_comboBoxSelection, 0, wxBOTTOM | wxRIGHT | wxLEFT, 5);


	sbSizer2->Add(bSizer49, 1, wxEXPAND, 5);


	bSizer39->Add(sbSizer2, 0, wxALL | wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Levels Options")), wxVERTICAL);

	wxBoxSizer* bSizer491;
	bSizer491 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticText461;
	m_staticText461 = new wxStaticText(this, wxID_ANY, wxT("Level Count :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText461->Wrap(-1);
	bSizer491->Add(m_staticText461, 0, wxALL, 5);

	m_comboBoxLevelCount = new wxComboBox(this, wxID_ANY, wxT("5 Levels"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_comboBoxLevelCount->Append(wxT("1 Levels"));
	m_comboBoxLevelCount->Append(wxT("2 Levels"));
	m_comboBoxLevelCount->Append(wxT("3 Levels"));
	m_comboBoxLevelCount->Append(wxT("4 Levels"));
	m_comboBoxLevelCount->Append(wxT("5 Levels"));
	m_comboBoxLevelCount->Append(wxT("6 Levels"));
	m_comboBoxLevelCount->Append(wxT("7 Levels"));
	m_comboBoxLevelCount->Append(wxT("8 Levels"));
	m_comboBoxLevelCount->SetSelection(4);
	bSizer491->Add(m_comboBoxLevelCount, 0, wxBOTTOM | wxRIGHT | wxLEFT, 5);


	sbSizer3->Add(bSizer491, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer40111;
	bSizer40111 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextSB11;
	m_staticTextSB11 = new wxStaticText(this, wxID_ANY, wxT("1L :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB11->Wrap(-1);
	m_staticTextSB11->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40111->Add(m_staticTextSB11, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

	m_sliderL1 = new wxSlider(this, wxID_ANY, 10, 5, 100, wxDefaultPosition, wxDefaultSize, 0);
	bSizer40111->Add(m_sliderL1, 1, 0, 5);


	sbSizer3->Add(bSizer40111, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer40112;
	bSizer40112 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextSB12;
	m_staticTextSB12 = new wxStaticText(this, wxID_ANY, wxT("2L :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB12->Wrap(-1);
	m_staticTextSB12->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40112->Add(m_staticTextSB12, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

	m_sliderL2 = new wxSlider(this, wxID_ANY, 20, 5, 100, wxDefaultPosition, wxDefaultSize, 0);
	bSizer40112->Add(m_sliderL2, 1, 0, 5);


	sbSizer3->Add(bSizer40112, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer40113;
	bSizer40113 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextSB13;
	m_staticTextSB13 = new wxStaticText(this, wxID_ANY, wxT("3L :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB13->Wrap(-1);
	m_staticTextSB13->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40113->Add(m_staticTextSB13, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

	m_sliderL3 = new wxSlider(this, wxID_ANY, 40, 5, 100, wxDefaultPosition, wxDefaultSize, 0);
	bSizer40113->Add(m_sliderL3, 1, 0, 5);


	sbSizer3->Add(bSizer40113, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer40114;
	bSizer40114 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextSB14;
	m_staticTextSB14 = new wxStaticText(this, wxID_ANY, wxT("4L :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB14->Wrap(-1);
	m_staticTextSB14->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40114->Add(m_staticTextSB14, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

	m_sliderL4 = new wxSlider(this, wxID_ANY, 70, 5, 100, wxDefaultPosition, wxDefaultSize, 0);
	bSizer40114->Add(m_sliderL4, 1, 0, 5);


	sbSizer3->Add(bSizer40114, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer40115;
	bSizer40115 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextSB15;
	m_staticTextSB15 = new wxStaticText(this, wxID_ANY, wxT("5L :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB15->Wrap(-1);
	m_staticTextSB15->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40115->Add(m_staticTextSB15, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

	m_sliderL5 = new wxSlider(this, wxID_ANY, 100, 5, 100, wxDefaultPosition, wxDefaultSize, 0);
	bSizer40115->Add(m_sliderL5, 1, 0, 5);


	sbSizer3->Add(bSizer40115, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer40116;
	bSizer40116 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextSB16;
	m_staticTextSB16 = new wxStaticText(this, wxID_ANY, wxT("6L :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB16->Wrap(-1);
	m_staticTextSB16->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40116->Add(m_staticTextSB16, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

	m_sliderL6 = new wxSlider(this, wxID_ANY, 100, 5, 100, wxDefaultPosition, wxDefaultSize, 0);
	bSizer40116->Add(m_sliderL6, 1, 0, 5);


	sbSizer3->Add(bSizer40116, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer40117;
	bSizer40117 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextSB17;
	m_staticTextSB17 = new wxStaticText(this, wxID_ANY, wxT("7L :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB17->Wrap(-1);
	m_staticTextSB17->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40117->Add(m_staticTextSB17, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

	m_sliderL7 = new wxSlider(this, wxID_ANY, 100, 5, 100, wxDefaultPosition, wxDefaultSize, 0);
	bSizer40117->Add(m_sliderL7, 1, 0, 5);


	sbSizer3->Add(bSizer40117, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer40118;
	bSizer40118 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticTextSB18;
	m_staticTextSB18 = new wxStaticText(this, wxID_ANY, wxT("8L :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextSB18->Wrap(-1);
	m_staticTextSB18->SetFont(wxFont(10, 74, 90, 90, false, wxT("Arial")));

	bSizer40118->Add(m_staticTextSB18, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

	m_sliderL8 = new wxSlider(this, wxID_ANY, 100, 5, 100, wxDefaultPosition, wxDefaultSize, 0);
	bSizer40118->Add(m_sliderL8, 1, 0, 5);


	sbSizer3->Add(bSizer40118, 0, wxEXPAND, 5);


	bSizer39->Add(sbSizer3, 1, wxEXPAND | wxALL, 5);


	this->SetSizer(bSizer39);
	this->Layout();

	this->Centre(wxBOTH);


	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("size of world:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	m_staticText1->SetFont(wxFont(13, 70, 90, 90, false, wxT("ËÎÌו")));

	bSizer2->Add(m_staticText1, 0, wxALL, 5);

	_uiWorldSize = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(_uiWorldSize, 0, wxBOTTOM | wxRIGHT | wxLEFT, 5);


	bSizer1->Add(bSizer2, 0, 0, 5);

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticText11;
	m_staticText11 = new wxStaticText(this, wxID_ANY, wxT("far of camera:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText11->Wrap(-1);
	m_staticText11->SetFont(wxFont(13, 70, 90, 90, false, wxT("ËÎÌו")));

	bSizer21->Add(m_staticText11, 0, wxALL, 5);

	_uiCameraFar = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer21->Add(_uiCameraFar, 0, wxBOTTOM | wxRIGHT | wxLEFT, 5);

	bSizer1->Add(bSizer21, 0, 0, 5);

	_uiShowCascade = new wxCheckBox(this, wxID_ANY, wxT("show cascade "), wxDefaultPosition, wxDefaultSize, 0);
	_uiShowCascade->SetValue(false);
	bSizer1->Add(_uiShowCascade, 0, wxALL, 5);

	_uiMesh = new wxComboBox(this, wxID_ANY, wxT("test"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	_uiMesh->Append(wxT("test"));
	_uiMesh->Append(wxT("role"));
	_uiMesh->Append(wxT("building"));
	_uiMesh->Append(wxT("city"));
	_uiMesh->SetSelection(0);
	bSizer1->Add(_uiMesh, 0, wxALL | wxEXPAND , 5);

	bSizer39->Add(bSizer1);

	// Connect Events
	_uiWorldSize->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(fbCascadePane::on_world_size_changed), NULL, this);
	_uiCameraFar->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(fbCascadePane::on_camera_far_changed), NULL, this);
	_uiShowCascade->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fbCascadePane::on_show_cascade), NULL, this);
	_uiMesh->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fbCascadePane::on_mesh_changed), NULL, this);

	// Connect Events
	m_checkBoxVC->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fbCascadePane::OnVCChecked), NULL, this);
	m_sliderTS->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(fbCascadePane::OnTSChanged), NULL, this);
	m_sliderSB->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(fbCascadePane::OnSBChanged), NULL, this);
	m_checkBoxCB->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fbCascadePane::OnCBChecked), NULL, this);
	m_sliderCB->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(fbCascadePane::OnCBChanged), NULL, this);
	m_comboBoxSelection->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fbCascadePane::OnSelectionChanged), NULL, this);
	m_comboBoxLevelCount->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fbCascadePane::OnLevelCountChanged), NULL, this);
	m_sliderL1->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL1Changed), NULL, this);
	m_sliderL2->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL2Changed), NULL, this);
	m_sliderL3->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL3Changed), NULL, this);
	m_sliderL4->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL4Changed), NULL, this);
	m_sliderL5->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL5Changed), NULL, this);
	m_sliderL6->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL6Changed), NULL, this);
	m_sliderL7->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL7Changed), NULL, this);
	m_sliderL8->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL8Changed), NULL, this);
}

fbCascadePane::~fbCascadePane()
{
	// Disconnect Events
	m_checkBoxVC->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fbCascadePane::OnVCChecked), NULL, this);
	m_sliderTS->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(fbCascadePane::OnTSChanged), NULL, this);
	m_sliderSB->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(fbCascadePane::OnSBChanged), NULL, this);
	m_checkBoxCB->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fbCascadePane::OnCBChecked), NULL, this);
	m_sliderCB->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(fbCascadePane::OnCBChanged), NULL, this);
	m_comboBoxSelection->Disconnect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fbCascadePane::OnSelectionChanged), NULL, this);
	m_comboBoxLevelCount->Disconnect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fbCascadePane::OnLevelCountChanged), NULL, this);
	m_sliderL1->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL1Changed), NULL, this);
	m_sliderL2->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL2Changed), NULL, this);
	m_sliderL3->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL3Changed), NULL, this);
	m_sliderL4->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL4Changed), NULL, this);
	m_sliderL5->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL5Changed), NULL, this);
	m_sliderL6->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL6Changed), NULL, this);
	m_sliderL7->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL7Changed), NULL, this);
	m_sliderL8->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(fbCascadePane::OnL8Changed), NULL, this);

	_uiWorldSize->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(fbCascadePane::on_world_size_changed), NULL, this);
	_uiCameraFar->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(fbCascadePane::on_camera_far_changed), NULL, this);
	_uiShowCascade->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fbCascadePane::on_show_cascade), NULL, this);
	_uiMesh->Disconnect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fbCascadePane::on_mesh_changed), NULL, this);
}

