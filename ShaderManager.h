#pragma once

#include <map>
#include <string>
#include "Shaders.h"
#include "Texture.h"
//#include "ConstantBuffer.h"
#include "ShaderReflection.h"
#include "DXCore.h"


class ShaderManager {
public:
	std::map<std::string, Shader*> shaders;

	void init() {

	}

	void addShader(std::string id, Shader* shader) {
		shaders.insert({ id, shader });
	}

	void removeShader(std::string id) {
		shaders.erase(shaders.find(id));
	}

	/*Shader changeShader(DXCore& core, std::string& shaderKey) {
		auto it = shaders.find(shaderKey);
		if (it != shaders.end()) {
			it->second.apply(core);
		}
	}*/

	void apply(DXCore* core, std::string shaderKey) {
		auto it = shaders.find(shaderKey);
		if (it != shaders.end()) {
			it->second->apply(core);
		}
	}

	void apply(DXCore* core, const std::string& shaderKey, const void* buffer, unsigned int sizeInBytes) {
		auto it = shaders.find(shaderKey);
		if (it != shaders.end()) {
			it->second->apply(core);
		}
	}

	void updateConstantVS(std::string shaderName, std::string constantBufferName, std::string variableName, void* data) {
		shaders[shaderName]->updateConstant(constantBufferName, variableName, data, shaders[shaderName]->vsConstantBuffers);
		//shaders[shaderName].vsConstantBuffers
	}

	void updateConstantPS(std::string shaderName, std::string constantBufferName, std::string variableName, void* data) {
		shaders[shaderName]->updateConstant(constantBufferName, variableName, data, shaders[shaderName]->psConstantBuffers);
	}

	void bindTexturePS(DXCore& core, std::string name, ID3D11ShaderResourceView* srv) {
		core.devicecontext->PSSetShaderResources(shaders[name]->textureBindPointsPS[name], 1, &srv);
	}
	
};

class TextureManager {
public:
	std::map<std::string, Texture*> textures;

	void init() {

	}

	void addTexture(std::string id, Texture* texture) {
		textures.insert({ id, texture });
	}

	void removeTexture(std::string id) {
		textures.erase(textures.find(id));
	}

	ID3D11ShaderResourceView* find(std::string id) {
		auto it = textures.find(id);
		if (it != textures.end()) {
			return it->second->srv;
		}

		return 0;
	}
};

class ShadowMap {
public:
	Shader* staticShadow = new Shader;
	Shader* animatedShadow = new Shader;

	Texture* shadowmap = new Texture;
	ID3D11DepthStencilView* depthStencilView;

	D3D11_VIEWPORT shadowmapViewport;

	void init(DXCore* core, unsigned int width, unsigned int height, ShaderManager& shaders, TextureManager& textures) {
		staticShadow->init(core, "Shaders/vs_shadowmap_static.txt", "Shaders/ps_shadowmap.txt", 2);
		shaders.addShader("static_shadow", staticShadow);

		animatedShadow->init(core, "Shaders/vs_shadowmap_animated.txt", "Shaders/ps_shadowmap.txt", 1);
		shaders.addShader("animated_shadow", animatedShadow);

		//shadowmap->init(core, width, height, 0, DXGI_FORMAT_R32_FLOAT, 0, true);
		//textures.addTexture("shadowmap", shadowmap);

		D3D11_TEXTURE2D_DESC dsvDesc;
		dsvDesc.Width = width;
		dsvDesc.Height = height;
		dsvDesc.MipLevels = 1;
		dsvDesc.ArraySize = 1;
		//dsvDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		dsvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		dsvDesc.SampleDesc.Count = 1;
		dsvDesc.SampleDesc.Quality = 0;
		dsvDesc.Usage = D3D11_USAGE_DEFAULT;
		dsvDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		dsvDesc.CPUAccessFlags = 0;
		dsvDesc.MiscFlags = 0;

		core->device->CreateTexture2D(&dsvDesc, NULL, &shadowmap->texture);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		dsDesc.Flags = 0;
		dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
		//dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Texture2D.MipSlice = 0;

		core->device->CreateDepthStencilView(shadowmap->texture, &dsDesc, &depthStencilView);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		//srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		core->device->CreateShaderResourceView(shadowmap->texture, &srvDesc, &shadowmap->srv);

		shadowmapViewport.Width = width;
		shadowmapViewport.Height = height;
		shadowmapViewport.MinDepth = 0.0f;
		shadowmapViewport.MaxDepth = 1.0f;
		shadowmapViewport.TopLeftX = 0;
		shadowmapViewport.TopLeftY = 0;
	}

	void setupShadowPass(DXCore* core) {

		ID3D11Debug* debugLayer = nullptr;
		core->device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugLayer);

		ID3D11ShaderResourceView* nullSRV[4] = { nullptr, nullptr, nullptr, nullptr };
		core->devicecontext->PSSetShaderResources(0, 4, nullSRV);

		core->devicecontext->RSSetViewports(1, &shadowmapViewport);
		//core->devicecontext->OMSetRenderTargets(1, &shadowmap->rtv, depthStencilView);
		core->devicecontext->OMSetRenderTargets(0, NULL, depthStencilView);
		//core->devicecontext->ClearRenderTargetView(shadowmap->rtv, &ClearColor);
		core->devicecontext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
};

class Reflection {
public:
	Texture* reflection = new Texture;
	Shader* staticReflectionShader = new Shader;
	Shader* animatedReflectionShader = new Shader;

	Texture* depthbuffer = new Texture;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT viewport;

	void init(DXCore* core, int width, TextureManager& textures, ShaderManager& shaders) {
		reflection->init(core, 1024, 700, 0, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0, true);
		textures.addTexture("reflection", reflection);

		staticReflectionShader->init(core, "Shaders/vs_reflection_static.txt", "Shaders/ps_reflection.txt", 0);
		shaders.addShader("static_reflection", staticReflectionShader);
		animatedReflectionShader->init(core, "Shaders/vs_reflection_animated.txt", "Shaders/ps_reflection.txt", 1);
		shaders.addShader("animated_reflection", animatedReflectionShader);

		D3D11_TEXTURE2D_DESC dsvDesc;
		dsvDesc.Width = 1024;
		dsvDesc.Height = 700;
		dsvDesc.MipLevels = 1;
		dsvDesc.ArraySize = 1;
		//dsvDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		dsvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		dsvDesc.SampleDesc.Count = 1;
		dsvDesc.SampleDesc.Quality = 0;
		dsvDesc.Usage = D3D11_USAGE_DEFAULT;
		dsvDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsvDesc.CPUAccessFlags = 0;
		dsvDesc.MiscFlags = 0;

		HRESULT hr = core->device->CreateTexture2D(&dsvDesc, NULL, &depthbuffer->texture);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		dsDesc.Flags = 0;
		dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Texture2D.MipSlice = 0;

		core->device->CreateDepthStencilView(depthbuffer->texture, &dsDesc, &depthStencilView);

		viewport.Width = 1024;
		viewport.Height = 700;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
	}

	void setupPass(DXCore* core) {
		float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
		core->devicecontext->RSSetViewports(1, &viewport);
		core->devicecontext->OMSetRenderTargets(1, &reflection->rtv, depthStencilView);
		core->devicecontext->ClearRenderTargetView(reflection->rtv, ClearColor);
		core->devicecontext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
};

class GBuffer {
public:
	ID3D11RenderTargetView* renderTargetViews[3];
	ID3D11ShaderResourceView* shaderResourceViews[4];

	void init(DXCore* core, Window& window, TextureManager& textures, ShadowMap& shadowmap) {
		Texture* albedo = new Texture;
		albedo->init(core, window.width, window.height, 0, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0, true);
		textures.addTexture("albedo", albedo);

		Texture* normal = new Texture;
		normal->init(core, window.width, window.height, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, true);
		textures.addTexture("normal", normal);

		Texture* worldPos = new Texture;
		worldPos->init(core, window.width, window.height, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, true);
		textures.addTexture("world_pos", worldPos);

		renderTargetViews[0] = albedo->rtv;
		renderTargetViews[1] = normal->rtv;
		renderTargetViews[2] = worldPos->rtv;

		shaderResourceViews[0] = albedo->srv;
		shaderResourceViews[1] = normal->srv;
		shaderResourceViews[2] = worldPos->srv;
		shaderResourceViews[3] = shadowmap.shadowmap->srv;
	}

	void setRenderTargets(DXCore* core) {
		float ClearColor[4] = { 0.0f, 0.0f, 0.5f, 0.f };
		core->devicecontext->RSSetViewports(1, &core->viewport);
		core->devicecontext->OMSetRenderTargets(3, renderTargetViews, core->depthStencilView);

		core->devicecontext->ClearRenderTargetView(renderTargetViews[0], ClearColor);
		core->devicecontext->ClearRenderTargetView(renderTargetViews[1], ClearColor);
		core->devicecontext->ClearRenderTargetView(renderTargetViews[2], ClearColor);
		core->devicecontext->ClearDepthStencilView(core->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
};