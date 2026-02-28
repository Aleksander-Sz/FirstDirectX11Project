#include "dxApplication.h"

using namespace mini;
DxApplication::DxApplication(HINSTANCE hInstance) : WindowApplication(hInstance), m_device(m_window) 
{
	ID3D11Texture2D* temp = nullptr;
	m_device.swapChain()->GetBuffer(0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&temp));
	const dx_ptr<ID3D11Texture2D> backTexture{ temp };
	m_backBuffer = m_device.CreateRenderTargetView(backTexture);

	SIZE wndSize = m_window.getClientSize();
	m_depthBuffer = m_device.CreateDepthStencilView(wndSize);
	auto backBuffer = m_backBuffer.get();
	m_device.context()->OMSetRenderTargets(1, &backBuffer, m_depthBuffer.get());
	Viewport viewport{ wndSize };
	m_device.context()->RSSetViewports(1, &viewport);
	
}
int DxApplication::MainLoop()
{
	MSG msg{};
	do
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
			m_device.swapChain()->Present(0, 0);
		}
	} while (msg.message != WM_QUIT);
	return msg.wParam;
}
void DxApplication::Update() {};
void DxApplication::Render()
{
	const float clearColor[] = { 0.5f,0.5f,1.0f,1.0f };
	m_device.context()->ClearRenderTargetView(m_backBuffer.get(), clearColor);
	m_device.context()->ClearDepthStencilView(m_depthBuffer.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	ID3D11Buffer* buffers[] = { m_device.vertexBuffer() };
	UINT strides[] = {sizeof(Vertex)};
	UINT offsets[] = {0};
	m_device.context()->IASetVertexBuffers(0, 1, buffers, strides, offsets);
	m_device.context()->IASetInputLayout(m_device.layout());
	m_device.context()->VSSetShader(m_device.vertexShader(), nullptr, 0);
	m_device.context()->PSSetShader(m_device.pixelShader(), nullptr, 0);
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_device.context()->Draw(3, 0);
}