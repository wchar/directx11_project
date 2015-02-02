#include "D3DPanel.h"

BEGIN_EVENT_TABLE(D3DPanel, wxPanel)
EVT_MOUSE_EVENTS(D3DPanel::onMouseEvent)
EVT_SIZE(D3DPanel::onSize)
END_EVENT_TABLE()

D3DPanel::D3DPanel(wxWindow* parent)
: wxPanel(parent)
{
	WE::gD3D()->create(this->GetHandle());

	_renderer = new WE::Renderer();
	_camera = new WE::ModelViewerCamera();

	auto mesh = WE::Mesh::createFromFbx(L"mesh/cub.fbx");
	_renderer->setMesh(mesh);

	_timer.SetOwner(this, wxID_ANY);
	_timer.Start(1000);
	this->Connect(wxID_ANY, wxEVT_TIMER, wxTimerEventHandler(D3DPanel::tick));
}

D3DPanel::~D3DPanel()
{
	SAFE_DELETE(_camera);

	this->Disconnect(wxID_ANY, wxEVT_TIMER, wxTimerEventHandler(D3DPanel::tick));
}

void D3DPanel::onSize(wxSizeEvent& evt)
{
	this->SetSize(evt.GetSize());
	//_renderer->resizeWindow();
}

void D3DPanel::onMouseEvent(wxMouseEvent& evt)
{
	wxPoint pt(evt.GetPosition());

	//if (evt.LeftDown())
	//{
	//	_camera->began(pt.x, pt.y);
	//	SetFocus();
	//}
	//else if (evt.Dragging() && evt.LeftIsDown())
	//{
	//	_camera->moved(pt.x, pt.y);
	//}
	//else if (evt.LeftUp())
	//{
	//	_camera->ended();
	//}

	//if (evt.GetWheelRotation() != 0)
	//{
	//	if (evt.GetWheelRotation() > 0)
	//		_camera->closer();
	//	else
	//		_camera->further();
	//}
}

void D3DPanel::tick(wxTimerEvent& event)
{
	_renderer->onFrameRender();
}