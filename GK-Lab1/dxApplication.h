#pragma once
#include "windowApplication.h"
#include "dxDevice.h"
#include <directxmath.h>
#include <profileapi.h>
#include <windowsx.h>

class DxApplication : public mini::WindowApplication {
public:
	explicit DxApplication(HINSTANCE hInstance);
protected:
	int MainLoop() override;
    bool ProcessMessage(mini::WindowMessage& msg) override;
private:
	void Render();
	void Update();


	DxDevice m_device;
	mini::dx_ptr<ID3D11RenderTargetView> m_backBuffer;

	mini::dx_ptr<ID3D11DepthStencilView> m_depthBuffer;
	mini::dx_ptr<ID3D11Buffer> m_vertexBuffer;
	mini::dx_ptr<ID3D11VertexShader> m_vertexShader;
	mini::dx_ptr<ID3D11PixelShader> m_pixelShader;
	mini::dx_ptr<ID3D11InputLayout> m_layout;

	bool buttonDown = false;
	bool isFirstMovement = false;
	short lastX;
	short lastY;
	float pitch;
	float yaw;
	LARGE_INTEGER counterFrequency;
	float rotationY;
	DirectX::XMFLOAT4X4 m_modelMtx, m_viewMtx, m_projMtx;
	mini::dx_ptr<ID3D11Buffer> m_cbMVP;
};