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