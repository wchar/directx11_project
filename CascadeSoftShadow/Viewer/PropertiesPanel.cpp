#include "PropertiesPanel.h"

///////////////////////////////////////////////////////////////////////////

CascadePane::CascadePane(wxWindow* parent, D3DPanel* d3dPanel, WE::CascadeManager* pCascadeManager)
	: fbCascadePane(parent)
{
	_cascadeManager = pCascadeManager;
	_d3dPanel = d3dPanel;

	_uiCameraFar->SetLabelText(wxLongLong(300).ToString());
	_uiWorldSize->SetLabelText(wxLongLong(1000).ToString());

	m_checkBoxVC->Hide();
}


CascadePane::~CascadePane()
{

}

void CascadePane::OnVCChecked(wxCommandEvent& event)
{
	_cascadeManager->cascadeConfig.visualizeCascades = m_checkBoxVC->GetValue() ? 1 : 0;
}

void CascadePane::OnTSChanged(wxScrollEvent& event)
{
	int iBufferSize = m_sliderTS->GetValue();
	m_staticTextTS->SetLabel(wxString::FromDouble(iBufferSize));

	_cascadeManager->cascadeConfig.bufferSize = iBufferSize;
	_cascadeManager->recreateCascadeBuffers();
}

void CascadePane::OnSBChanged(wxScrollEvent& event)
{
	int iSBSize = m_sliderSB->GetValue() * 2 + 1;

	m_staticTextSB->SetLabel(wxString::FromDouble(iSBSize));

	_cascadeManager->cascadeConfig.shadowBlurSize = iSBSize;
}

void CascadePane::OnCBChecked(wxCommandEvent& event)
{
	_cascadeManager->cascadeConfig.blurBetweenCascades = m_checkBoxCB->GetValue() ? 1 : 0;
}

void CascadePane::OnCBChanged(wxScrollEvent& event)
{
	float fBlur = 0.0001 * m_sliderCB->GetValue();
	_cascadeManager->cascadeConfig.blurBetweenCascadesAmount = fBlur;

	m_staticTextCB->SetLabel(wxString::FromDouble(fBlur));
}

void CascadePane::OnSelectionChanged(wxCommandEvent& event)
{
	_cascadeManager->cascadeConfig.mapSelection = m_comboBoxSelection->GetSelection();
}

void CascadePane::OnLevelCountChanged(wxCommandEvent& event)
{
	auto lv = m_comboBoxLevelCount->GetSelection();
	if (0 < lv && lv <= 8)
	{
		_cascadeManager->cascadeConfig.cascadeLevels = lv;
		_cascadeManager->recreateCascadeBuffers();
	}
}

void CascadePane::OnAnyLevelChanged(int level)
{
	int levels[8];
	levels[0] = m_sliderL1->GetValue();
	levels[1] = m_sliderL2->GetValue();
	levels[2] = m_sliderL3->GetValue();
	levels[3] = m_sliderL4->GetValue();
	levels[4] = m_sliderL5->GetValue();
	levels[5] = m_sliderL6->GetValue();
	levels[6] = m_sliderL7->GetValue();
	levels[7] = m_sliderL8->GetValue();

	for (int i = 0; i < 8; i++)
	{
		if (i < level && levels[i] > levels[level])
		{
			levels[i] = levels[level];
		}
		if (i > level && levels[i] < levels[level])
		{
			levels[i] = levels[level];
		}
	}

	m_sliderL1->SetValue(levels[0]);
	m_sliderL2->SetValue(levels[1]);
	m_sliderL3->SetValue(levels[2]);
	m_sliderL4->SetValue(levels[3]);
	m_sliderL5->SetValue(levels[4]);
	m_sliderL6->SetValue(levels[5]);
	m_sliderL7->SetValue(levels[6]);
	m_sliderL8->SetValue(levels[7]);

	for (int i = 0; i < 8; i++)
	{
		_cascadeManager->cascadeConfig.cascadePartitionsZeroToOne[i] = levels[i];
	}
}

void CascadePane::on_world_size_changed(wxCommandEvent& event)
{
	auto text = _uiWorldSize->GetValue();
	long v;
	if (text.ToLong(&v))
	{
		_cascadeManager->setScenesSize(v);
	}
	else
	{
		int size = _cascadeManager->getScenesSize();
		auto tx = wxLongLong(size).ToString();
		_uiWorldSize->SetLabel(tx);
	}
}

void CascadePane::on_camera_far_changed(wxCommandEvent& event)
{
	auto text = _uiCameraFar->GetValue();
	long v;
	if (text.ToLong(&v))
	{
		_cascadeManager->setScenesSize(v);
	}
	else
	{
		int size = _cascadeManager->getScenesSize();
		auto tx = wxLongLong(size).ToString();
		_uiWorldSize->SetLabel(tx);
	}
}

void CascadePane::on_show_cascade(wxCommandEvent& event)
{
	_cascadeManager->cascadeConfig.visualizeCascades = _uiShowCascade->GetValue();
}

void CascadePane::on_mesh_changed(wxCommandEvent& event)
{
	_d3dPanel->setMesh(_uiMesh->GetSelection());
}