#include "dxDevice.h"
#include "exceptions.h"

DxDevice::DxDevice(const mini::Window& window)
{
	SwapChainDescription desc{
		window.getHandle(), window.getClientSize()
	};
	ID3D11Device* d = nullptr;
	ID3D11DeviceContext* dc = nullptr;
	IDXGISwapChain* sc = nullptr;

	auto hr = D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_DEBUG, nullptr, 0,
		D3D11_SDK_VERSION, &desc, &sc, &d, nullptr, &dc);
	m_device.reset(d);
	m_swapChain.reset(sc);
	m_context.reset(dc);
	if (FAILED(hr)) THROW_WINAPI;

	const auto vsBytes = LoadByteCode(L"VertexShader.cso");
	const auto psBytes = LoadByteCode(L"PixelShader.cso");
	ID3D11VertexShader* rawVertexShader;
	ID3D11PixelShader* rawPixelShader;
	m_device->CreateVertexShader(vsBytes.data(), vsBytes.size(), NULL, &rawVertexShader);
	m_device->CreatePixelShader(psBytes.data(), psBytes.size(), NULL, &rawPixelShader);
	m_vertexShader.reset(rawVertexShader);
	m_pixelShader.reset(rawPixelShader);
	const auto vertices = CreateTriangleVertices();
	m_vertexBuffer = CreateVertexBuffer(vertices);
	std::vector<D3D11_INPUT_ELEMENT_DESC> elements{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	ID3D11InputLayout* rawLayout = nullptr;
	hr = m_device->CreateInputLayout(elements.data(), elements.size(), vsBytes.data(), vsBytes.size(), &rawLayout);
	if (FAILED(hr)) THROW_WINAPI;

	m_layout.reset(rawLayout);
}

mini::dx_ptr<ID3D11RenderTargetView> DxDevice::CreateRenderTargetView(const mini::dx_ptr<ID3D11Texture2D>& texture) const
{
	ID3D11RenderTargetView* temp = nullptr;
	auto hr = m_device->CreateRenderTargetView(texture.get(), nullptr, &temp);
	mini::dx_ptr<ID3D11RenderTargetView> result{ temp };
	if (FAILED(hr)) THROW_WINAPI;
	return result;
}

mini::dx_ptr<ID3D11Texture2D> DxDevice::CreateTexture(const Texture2DDescription& desc) const
{
    ID3D11Texture2D* tex = nullptr;
    auto hr = m_device->CreateTexture2D(&desc, nullptr, &tex);
    mini::dx_ptr<ID3D11Texture2D> result{ tex };
    if (FAILED(hr)) THROW_WINAPI;
    return result;
}

mini::dx_ptr<ID3D11Buffer> DxDevice::CreateBuffer(const void* data, D3D11_BUFFER_DESC desc) const
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_SUBRESOURCE_DATA initData{};
	D3D11_SUBRESOURCE_DATA* pInitData = nullptr;

	if (data)
	{
		initData.pSysMem = data;
		pInitData = &initData;
	}

    // pass pInitData (may be nullptr) instead of always passing &initData
    auto hr = m_device->CreateBuffer(&desc, pInitData, &buffer);
	mini::dx_ptr<ID3D11Buffer> result{ buffer };
	if (FAILED(hr)) THROW_WINAPI;
	return result;
}

mini::dx_ptr<ID3D11DepthStencilView> DxDevice::CreateDepthStencilView(SIZE size) const
{
    // build a proper depth-stencil texture description
    auto desc = Texture2DDescription::DepthStencilDescription((UINT)size.cx, (UINT)size.cy);
    mini::dx_ptr<ID3D11Texture2D> texture = CreateTexture(desc);

    ID3D11DepthStencilView* dsv = nullptr;
    auto hr = m_device->CreateDepthStencilView(texture.get(), nullptr, &dsv);
    mini::dx_ptr<ID3D11DepthStencilView> result{ dsv };
    if (FAILED(hr)) THROW_WINAPI;
    return result;
}

std::vector<char> DxDevice::LoadByteCode(const std::wstring& filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) return {};
	return std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::vector<Vertex> DxDevice::CreateTriangleVertices()
{
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex{ -0.5f, -0.5f });
	vertices.push_back(Vertex{ -0.5f,  0.5f });
	vertices.push_back(Vertex{  0.5f, -0.5f });
	return vertices;
}