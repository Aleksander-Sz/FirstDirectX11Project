#include "dxStructures.h"

SwapChainDescription::SwapChainDescription(HWND wndHwnd, SIZE wndSize) : DXGI_SWAP_CHAIN_DESC{}
{
	BufferDesc.Width = wndSize.cx;
	BufferDesc.Height = wndSize.cy;
	BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	BufferDesc.RefreshRate.Denominator = 1;
	SampleDesc.Count = 1;
	BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	BufferCount = 1;
	OutputWindow = wndHwnd;
	Windowed = true;
}

Texture2DDescription::Texture2DDescription(UINT width, UINT height) : D3D11_TEXTURE2D_DESC{}
{
    Width = width;
    Height = height;
    MipLevels = 1;
    ArraySize = 1;
    Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SampleDesc.Count = 1;
    SampleDesc.Quality = 0;
    Usage = D3D11_USAGE_DEFAULT;
    BindFlags = 0;
    CPUAccessFlags = 0;
    MiscFlags = 0;
}

Texture2DDescription Texture2DDescription::DepthStencilDescription(UINT width, UINT height)
{
	Texture2DDescription desc(width, height);
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	return desc;
}

Viewport::Viewport(SIZE wndSize)
{
	TopLeftX = 0.0f;
	TopLeftY = 0.0f;
	Width = wndSize.cx;
	Height = wndSize.cy;
	MinDepth = 0.0f;
	MaxDepth = 1.0f;
}

BufferDescription::BufferDescription(UINT bindFlags, size_t byteWidth)
{
	Usage = D3D11_USAGE_IMMUTABLE;
	BindFlags = bindFlags;
	ByteWidth = byteWidth;
	CPUAccessFlags = 0;
	MiscFlags = 0;
	StructureByteStride = 0;
}

Vertex::Vertex(float x, float y, float z, float r, float g, float b)
{
	position = DirectX::XMFLOAT3(x, y, z);
	color = DirectX::XMFLOAT3(r, g, b);
}