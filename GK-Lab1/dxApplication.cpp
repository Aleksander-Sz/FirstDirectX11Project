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

	XMStoreFloat4x4(&m_model1Mtx, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_model2Mtx, DirectX::XMMatrixTranslation(-10.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&m_viewMtx, DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(-30)) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 10.0f));
	XMStoreFloat4x4(&m_projMtx, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45),
		static_cast<float>(wndSize.cx) / wndSize.cy, 0.1f, 100.0f));
	m_cbMVP = m_device.CreateConstantBuffer<DirectX::XMFLOAT4X4>();

	QueryPerformanceFrequency(&counterFrequency);
	//buttonDown = false;
	//pitch = 0.0f;
	//yaw = 0.0f;
/*	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE; // Wy³¹czamy odrzucanie œcianek
	rasterDesc.FrontCounterClockwise = FALSE;

	ID3D11RasterizerState* pRasterizerState;
	m_device->CreateRasterizerState(&rasterDesc, &pRasterizerState);
	m_device.context()->RSSetState(pRasterizerState);*/
	
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
void DxApplication::Update()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	rotationY = 0.01 * (int)counter.QuadPart / (int)counterFrequency.QuadPart;
	DirectX::XMStoreFloat4x4(&m_model1Mtx, DirectX::XMMatrixRotationY(DirectX::XMConvertToDegrees(rotationY)));
	XMStoreFloat4x4(&m_viewMtx, DirectX::XMMatrixRotationY(yaw) *
		DirectX::XMMatrixRotationX(pitch) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, zoom));
}
bool DxApplication::ProcessMessage(mini::WindowMessage& msg)
{
	switch (msg.message)
	{
	case WM_MOUSEMOVE:
	{
		signed short x = GET_X_LPARAM(msg.lParam);
		signed short y = GET_Y_LPARAM(msg.lParam);
		if (isFirstMovement)
		{
			lastX = x;
			lastY = y;
			isFirstMovement = false;
			msg.result = 0;
			return true;
		}
		if (leftButtonDown)
		{
			short dx = x - lastX;
			short dy = y - lastY;
			yaw -= dx * 0.03f;
			if (yaw < -3.14159f)
				yaw = -3.14159f;
			if (yaw > 3.14159f)
				yaw = 3.14159f;
			pitch -= dy * 0.03f;
			if (pitch < -3.14159f)
				pitch = -3.14159f;
			if (pitch > 3.14159f)
				pitch = 3.14159f;
			lastX = x;
			lastY = y;
			msg.result = 0;
			return true;
		}
		if (rightButtonDown)
		{
			short dy = y - lastY;
			zoom += dy * 0.05f;
			if (zoom > 50.0f)
				zoom = 50.0f;
			if (zoom < 0.0f)
				zoom = 0.0f;
			lastX = x;
			lastY = y;
			msg.result = 0;
			return true;
		}
		return true;
	}
	case WM_LBUTTONDOWN:
		leftButtonDown = true;
		isFirstMovement = true;
		msg.result = 0;
		return true;
	case WM_LBUTTONUP:
		leftButtonDown = false;
		msg.result = 0;
		return true;
	case WM_RBUTTONDOWN:
		rightButtonDown = true;
		isFirstMovement = true;
		msg.result = 0;
		return true;
	case WM_RBUTTONUP:
		rightButtonDown = false;
		msg.result = 0;
		return true;
	case WM_MOUSEWHEEL:
	{
		int delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
		msg.result = 0;
		return true;
	}
	}
	return false;
}

void DxApplication::Render()
{
	const float clearColor[] = { 0.5f,0.5f,1.0f,1.0f };
	m_device.context()->ClearRenderTargetView(m_backBuffer.get(), clearColor);
	m_device.context()->ClearDepthStencilView(m_depthBuffer.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	ID3D11Buffer* buffers[] = { m_device.vertexBuffer() };
	UINT strides[] = {sizeof(Vertex)};
	UINT offsets[] = {0};
	m_device.context()->IASetVertexBuffers(0, 1, buffers, strides, offsets);
	m_device.context()->IASetIndexBuffer(m_device.indexBuffer(1), DXGI_FORMAT_R16_UINT, 0);
	m_device.context()->IASetInputLayout(m_device.layout());
	m_device.context()->VSSetShader(m_device.vertexShader(), nullptr, 0);
	m_device.context()->PSSetShader(m_device.pixelShader(), nullptr, 0);
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11Buffer* cbs[] = { m_cbMVP.get() };
	m_device.context()->VSSetConstantBuffers(0, 1, cbs);

	D3D11_MAPPED_SUBRESOURCE res;
	DirectX::XMMATRIX mvp;

	mvp = XMLoadFloat4x4(&m_model1Mtx) * XMLoadFloat4x4(&m_viewMtx) * XMLoadFloat4x4(&m_projMtx);
	m_device.context()->Map(m_cbMVP.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	memcpy(res.pData, &mvp, sizeof(DirectX::XMMATRIX));
	m_device.context()->Unmap(m_cbMVP.get(), 0);
	m_device.context()->DrawIndexed(36, 0, 0);

	mvp = XMLoadFloat4x4(&m_model2Mtx) * XMLoadFloat4x4(&m_viewMtx) * XMLoadFloat4x4(&m_projMtx);
	m_device.context()->Map(m_cbMVP.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	memcpy(res.pData, &mvp, sizeof(DirectX::XMMATRIX));
	m_device.context()->Unmap(m_cbMVP.get(), 0);
	m_device.context()->DrawIndexed(36, 0, 0);

	m_device.context()->IASetIndexBuffer(m_device.indexBuffer(2), DXGI_FORMAT_R16_UINT, 0);
	mvp = DirectX::XMMatrixTranslation(4.0f, 0.0f, 4.0f) * XMLoadFloat4x4(&m_viewMtx) * XMLoadFloat4x4(&m_projMtx);
	m_device.context()->Map(m_cbMVP.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	memcpy(res.pData, &mvp, sizeof(DirectX::XMMATRIX));
	m_device.context()->Unmap(m_cbMVP.get(), 0);
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_device.context()->DrawIndexed(6, 0, 0);
}

