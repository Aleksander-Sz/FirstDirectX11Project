#pragma once
#include "dxptr.h"
#include "window.h"
#include "dxStructures.h"
#include <vector>
#include <fstream>
#include <iostream>

class DxDevice {
public:
	explicit DxDevice(const mini::Window& window);
	ID3D11Device* operator->() const { return m_device.get(); }
	mini::dx_ptr<ID3D11RenderTargetView> CreateRenderTargetView(const mini::dx_ptr<ID3D11Texture2D>& texture) const;
	mini::dx_ptr<ID3D11Texture2D> CreateTexture(const Texture2DDescription& desc) const;
	mini::dx_ptr<ID3D11DepthStencilView> CreateDepthStencilView(SIZE size) const;
	mini::dx_ptr<ID3D11Buffer> CreateBuffer(const void* data, D3D11_BUFFER_DESC desc) const;
	std::vector<char> LoadByteCode(const std::wstring& filename);

	template<class T> 
	inline mini::dx_ptr<ID3D11Buffer> CreateVertexBuffer(const std::vector<T>& vertices) const
	{
		auto desc = BufferDescription::VertexBufferDescription(vertices.size() * sizeof(T));
		return CreateBuffer(reinterpret_cast<const void*>(vertices.data()), desc);
	}

	std::vector<Vertex> CreateTriangleVertices();

	//getters
	const  mini::dx_ptr<ID3D11DeviceContext>& context() const
	{
		return m_context;
	}
	const mini::dx_ptr<IDXGISwapChain>& swapChain() const
	{
		return m_swapChain;
	}
	ID3D11Buffer* vertexBuffer() const
	{
		return m_vertexBuffer.get();
	}
	ID3D11InputLayout* layout() const
	{
		return m_layout.get();
	}
	ID3D11VertexShader* vertexShader() const
	{
		return m_vertexShader.get();
	}
	ID3D11PixelShader* pixelShader() const
	{
		return m_pixelShader.get();
	}

private:
	mini::dx_ptr<ID3D11Device> m_device;
	mini::dx_ptr<ID3D11DeviceContext> m_context;
	mini::dx_ptr<IDXGISwapChain> m_swapChain;
	mini::dx_ptr<ID3D11Buffer> m_vertexBuffer;
	mini::dx_ptr<ID3D11InputLayout> m_layout;
	mini::dx_ptr<ID3D11VertexShader> m_vertexShader;
	mini::dx_ptr<ID3D11PixelShader> m_pixelShader;
};
