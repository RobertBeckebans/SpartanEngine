/*
Copyright(c) 2016 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= LINKING =====================
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
//===============================

//= INCLUDES ==================
#include <d3d11.h>
#include "../../Math/Vector4.h"
#include <string>
//=============================

class D3D11Graphics
{
public:
	D3D11Graphics();
	~D3D11Graphics();

	void Initialize(HWND handle);
	bool CreateDepthStencilBuffer();
	bool CreateDepthStencil();
	bool CreateDepthStencilView();
	void Release();

	void Clear(const Directus::Math::Vector4& color);
	void Present();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	void SetFaceCullMode(D3D11_CULL_MODE cull);

	void SetBackBufferRenderTarget();

	void SetResolution(int width, int height);
	void SetViewport(int width, int height);
	void ResetViewport();

private:	
	DXGI_SWAP_CHAIN_DESC GetSwapchainDesc(HWND handle);

	D3D11_RASTERIZER_DESC m_rasterizerDesc;
	D3D11_DEPTH_STENCIL_DESC m_depthStencilDesc;
	D3D11_BLEND_DESC m_blendStateDesc;
	DXGI_MODE_DESC* m_displayModeList;
	int m_videoCardMemory;
	std::string m_videoCardDescription;
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilStateEnabled;
	ID3D11DepthStencilState* m_depthStencilStateDisabled;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterStateCullFront;
	ID3D11RasterizerState* m_rasterStateCullBack;
	ID3D11RasterizerState* m_rasterStateCullNone;
	D3D11_VIEWPORT m_viewport;
	ID3D11BlendState* m_alphaBlendingStateEnabled;
	ID3D11BlendState* m_alphaBlendingStateDisabled;
};