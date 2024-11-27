#pragma once

#include <map>
#include <string>
#include "Shaders.h"
//#include "ConstantBuffer.h"
#include "ShaderReflection.h"
#include "DXCore.h"


class ShaderManager {
public:
	std::map<std::string, Shader> shaders;

	void init() {

	}

	void addShader(std::string id, Shader& shader) {
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

	void apply(DXCore& core, std::string shaderKey) {
		auto it = shaders.find(shaderKey);
		if (it != shaders.end()) {
			it->second.apply(core);
		}
	}

	void apply(DXCore& core, const std::string& shaderKey, const void* buffer, unsigned int sizeInBytes) {
		auto it = shaders.find(shaderKey);
		if (it != shaders.end()) {
			it->second.apply(core);
		}
	}

	void updateConstantVS(std::string shaderName, std::string constantBufferName, std::string variableName, void* data) {
		shaders[shaderName].updateConstant(constantBufferName, variableName, data, shaders[shaderName].vsConstantBuffers);
		//shaders[shaderName].vsConstantBuffers
	}

	void updateConstantPS(std::string shaderName, std::string constantBufferName, std::string variableName, void* data) {
		shaders[shaderName].updateConstant(constantBufferName, variableName, data, shaders[shaderName].psConstantBuffers);
	}
	
};
