#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "DXCore.h"
#include "Shaders.h"
#include <string>

class Texture {
public:
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;

	void init(DXCore* core, int width, int height, int channels, DXGI_FORMAT format, unsigned char* data) {
		D3D11_TEXTURE2D_DESC texDesc;
		memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		memset(&initData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = data;
		initData.SysMemPitch = width * channels;
		core->device->CreateTexture2D(&texDesc, &initData, &texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		core->device->CreateShaderResourceView(texture, &srvDesc, &srv);
	}

	void load(DXCore* core, std::string textureFile) {
		int width = 0;
		int height = 0;
		int channels = 0;
		unsigned char* texels = stbi_load(textureFile.c_str(), &width, &height, &channels, 0);

		if (channels == 3) {
			channels = 4;
			unsigned char* texelsWithAlpha = new unsigned char[width * height * channels];

			for (int i = 0; i < (width * height); i++) {
				texelsWithAlpha[i * 4] = texels[i * 3];
				texelsWithAlpha[(i * 4) + 1] = texels[(i * 3) + 1];
				texelsWithAlpha[(i * 4) + 2] = texels[(i * 3) + 2];
				texelsWithAlpha[(i * 4) + 3] = 255;
			}
			// Initialize texture using width, height, channels, and texelsWithAlpha
			init(core, width, height, channels, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, texelsWithAlpha);
			delete[] texelsWithAlpha;

		} else {
			// Initialize texture using width, height, channels, and texels
			init(core, width, height, channels, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, texels);
		}

		stbi_image_free(texels);
	}
};

class Sampler {
public:
	ID3D11SamplerState* state;

	void init(DXCore* core) {
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		core->device->CreateSamplerState(&samplerDesc, &state);

		//Bind (should be made separate when using multiple sampling states)
		core->devicecontext->PSSetSamplers(0, 1, &state);
	}
};