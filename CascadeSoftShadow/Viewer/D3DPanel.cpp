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

	_meshs[0] = WE::Mesh::createFromFbx(L"..\\mesh\\test.fbx");
	_meshs[1] = WE::Mesh::createFromFbx(L"..\\mesh\\role.fbx");
	_meshs[2] = WE::Mesh::createFromFbx(L"..\\mesh\\building.fbx");
	_meshs[3] = WE::Mesh::createFromFbx(L"..\\mesh\\city.fbx");
	auto _camera = static_cast<WE::ModelViewerCamera*>(WE::gCamera());
	
	_renderer->setMesh(_meshs[0]);
}

D3DPanel::~D3DPanel()
{

}

void D3DPanel::onSize(wxSizeEvent& evt)
{
	this->SetSize(evt.GetSize());
	_renderer->resizeWindow();
}

void D3DPanel::onMouseEvent(wxMouseEvent& evt)
{
	this->SetFocus();
	
	wxPoint pt(evt.GetPosition());

	WE::Camera* tmpCamera = WE::gCamera();

	if (GetAsyncKeyState('L'))
		tmpCamera = WE::gLight();

	auto _camera = static_cast<WE::ModelViewerCamera*>(tmpCamera);
	if (evt.LeftDown())
	{
		_camera->began(pt.x, pt.y);
	}
	else if (evt.Dragging() && evt.LeftIsDown())
	{
		_camera->moved(pt.x, pt.y);
	}
	else if (evt.LeftUp())
	{
		_camera->ended();
	}

	if (evt.GetWheelRotation() != 0)
	{
		if (evt.GetWheelRotation() > 0)
			_camera->closer();
		else
			_camera->further();
	}
}

void D3DPanel::tick(function<void()> callback)
{
	_renderer->onFrameRender();
	callback();
}

void D3DPanel::setMesh(int i)
{
	_renderer->setMesh(_meshs[i]);
}