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

	D3D11_VIEWPORT shadowmapViewport;

	void init(DXCore* core, unsigned int width, unsigned int height, ShaderManager& shaders, TextureManager& textures) {
		staticShadow->init(core, "Shaders/vs_shadowmap_static.txt", "Shaders/ps_shadowmap.txt", 2);
		shaders.addShader("static_shadow", staticShadow);

		animatedShadow->init(core, "Shaders/vs_shadowmap_animated.txt", "Shaders/ps_shadowmap.txt", 3);
		shaders.addShader("animated_shadow", animatedShadow);

		shadowmap->init(core, width, height, 0, DXGI_FORMAT_R32_FLOAT, 0, true);
		textures.addTexture("shadowmap", shadowmap);

		shadowmapViewport.Width = width;
		shadowmapViewport.Height = height;
		shadowmapViewport.MinDepth = 0.0f;
		shadowmapViewport.MaxDepth = 1.0f;
		shadowmapViewport.TopLeftX = 0;
		shadowmapViewport.TopLeftY = 0;
	}

	void setupShadowPass(DXCore* core) {
		float ClearColor[4] = { 0.0f, 0.5f, 0.0f, 0.f };
		core->devicecontext->RSSetViewports(1, &shadowmapViewport);
		core->devicecontext->OMSetRenderTargets(1, &shadowmap->rtv, NULL);
		core->devicecontext->ClearRenderTargetView(shadowmap->rtv, ClearColor);
		//core->devicecontext->ClearDepthStencilView(core->depthStencilView, D3D11_CLEAR_DEPTH/* | D3D11_CLEAR_STENCIL*/, 1.0f, 0);
	}
};

class GBuffer {
public:
	ID3D11RenderTargetView* renderTargetViews[3];
	ID3D11ShaderResourceView* shaderResourceViews[3];

	void init(DXCore* core, Window& window, TextureManager& textures, ShadowMap& shadowmap) {
		Texture* albedo = new Texture;
		albedo->init(core, window.width, window.height, 0, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0, true);
		textures.addTexture("albedo", albedo);

		Texture* normal = new Texture;
		normal->init(core, window.width, window.height, 0, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0, true);
		textures.addTexture("normal", normal);

		Texture* tangent = new Texture;
		tangent->init(core, window.width, window.height, 0, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0, true);
		textures.addTexture("world_pos", tangent);

		renderTargetViews[0] = albedo->rtv;
		renderTargetViews[1] = normal->rtv;
		renderTargetViews[2] = tangent->rtv;

		shaderResourceViews[0] = shadowmap.shadowmap->srv;
		//shaderResourceViews[0] = albedo->srv;
		shaderResourceViews[1] = normal->srv;
		shaderResourceViews[2] = tangent->srv;
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