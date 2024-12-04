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
	ID3D11Texture2D* depthbuffer;

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
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
		dsvDesc.SampleDesc.Count = 1;
		dsvDesc.SampleDesc.Quality = 0;
		dsvDesc.Usage = D3D11_USAGE_DEFAULT;
		dsvDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsvDesc.CPUAccessFlags = 0;
		dsvDesc.MiscFlags = 0;

		device->CreateTexture2D(&dsvDesc, NULL, &depthbuffer);
		device->CreateDepthStencilView(depthbuffer, NULL, &depthStencilView);

		devicecontext->OMSetRenderTargets(1, &backbufferRenderTargetView, depthStencilView);

		D3D11_VIEWPORT viewport;
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

	void clear() {
		float ClearColour[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
		devicecontext->ClearRenderTargetView(backbufferRenderTargetView, ClearColour);
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