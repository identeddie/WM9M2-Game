#pragma once
#include "Adapter.h"

class DXCore {
public:
	Adapter adapterInst;

	ID3D11Device* device;
	ID3D11DeviceContext* devicecontext;
	IDXGISwapChain* swapchain;

	ID3D11RenderTargetView* backbufferRenderTargetView;
	ID3D11Texture2D* backbuffer;
	ID3D11DepthStencilView* depthStencilView;
	//ID3D11ShaderResourceView* depthSrv;
	ID3D11Texture2D* depthbuffer;
	D3D11_VIEWPORT viewport;

	//For rasterizer state
	ID3D11RasterizerState* rasterizerState;
	//ID3D11DepthStencilState* depthStencilState;
	//ID3D11BlendState* blendState;

	~DXCore() {
		backbuffer->Release();
		depthStencilView->Release();
		depthbuffer->Release();
		backbufferRenderTargetView->Release();
		swapchain->Release();
		devicecontext->Release();
		device->Release();
	}

	void init(int width, int height, HWND hwnd, bool window_fullscreen) {

		adapterInst.findAdapter();

		DXGI_SWAP_CHAIN_DESC sd;
		memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = window_fullscreen ? false : true;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		D3D_FEATURE_LEVEL fl;
		fl = D3D_FEATURE_LEVEL_11_0;

		D3D11CreateDeviceAndSwapChain(adapterInst.adapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			D3D11_CREATE_DEVICE_DEBUG,
			&fl,
			1,
			D3D11_SDK_VERSION,
			&sd,
			&swapchain,
			&device,
			NULL,
			&devicecontext);

		swapchain->SetFullscreenState(window_fullscreen, NULL);

		swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);

		device->CreateRenderTargetView(backbuffer, NULL, &backbufferRenderTargetView);

		D3D11_TEXTURE2D_DESC dsvDesc;
		dsvDesc.Width = width;
		dsvDesc.Height = height;
		dsvDesc.MipLevels = 1;
		dsvDesc.ArraySize = 1;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//dsvDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		dsvDesc.SampleDesc.Count = 1;
		dsvDesc.SampleDesc.Quality = 0;
		dsvDesc.Usage = D3D11_USAGE_DEFAULT;
		dsvDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL/* | D3D11_BIND_SHADER_RESOURCE*/;
		dsvDesc.CPUAccessFlags = 0;
		dsvDesc.MiscFlags = 0;

		HRESULT hr = device->CreateTexture2D(&dsvDesc, NULL, &depthbuffer);

		if (FAILED(hr)) {
			MessageBoxA(NULL, "Error", "zbuff creation", 0);
			exit(0);
		}

		/*D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		dsDesc.Flags = 0;
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Texture2D.MipSlice = 0;*/

		device->CreateDepthStencilView(depthbuffer, NULL, &depthStencilView);

		/*if (FAILED(hr)) {
			char errorMessage[256];
			sprintf_s(errorMessage, "CreateDepthStencilView failed with HRESULT: 0x%08X\n", hr);
			MessageBoxA(NULL, "Error", errorMessage, 0);
			exit(0);
		}*/

		/*D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;*/

		//hr = device->CreateShaderResourceView(depthbuffer, &srvDesc, &depthSrv);

		if (FAILED(hr)) {
			MessageBoxA(NULL, "Error", "shader resource creation", 0);
			exit(0);
		}

		//devicecontext->OMSetRenderTargets(0, NULL, depthStencilView);
		devicecontext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH /* | D3D11_CLEAR_STENCIL*/, 1.0f, 0);

		devicecontext->OMSetRenderTargets(1, &backbufferRenderTargetView, depthStencilView);

		viewport.Width = (float)width;
		viewport.Height = (float)height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		devicecontext->RSSetViewports(1, &viewport);

		//Setup rasterizer state

		D3D11_RASTERIZER_DESC rsdesc;
		ZeroMemory(&rsdesc, sizeof(D3D11_RASTERIZER_DESC));
		rsdesc.FillMode = D3D11_FILL_SOLID;
		rsdesc.CullMode = D3D11_CULL_NONE;
		device->CreateRasterizerState(&rsdesc, &rasterizerState);

		devicecontext->RSSetState(rasterizerState);

	}

	void setRenderTarget(ID3D11RenderTargetView* renderTargetView) {
		float ClearColor[4] = { 0.0f, 0.0f, 0.5f, 1.0f };
		devicecontext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		devicecontext->ClearRenderTargetView(renderTargetView, ClearColor);
		devicecontext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH/* | D3D11_CLEAR_STENCIL*/, 1.0f, 0);
	}

	void renderToBackbuffer() {
		setRenderTarget(backbufferRenderTargetView);
	}

	void clear() {
		float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
		devicecontext->ClearRenderTargetView(backbufferRenderTargetView, ClearColor);
		devicecontext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void present() {
		swapchain->Present(0, 0);
	}

	void close() {
		backbuffer->Release();
		depthStencilView->Release();
		depthbuffer->Release();
		backbufferRenderTargetView->Release();
		swapchain->Release();
		devicecontext->Release();
		device->Release();
	}
};