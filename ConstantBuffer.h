#pragma once

#include "DXCore.h"

//class ConstantBuffer {
//public:
//	ID3D11Buffer* constantBuffer;
//
//	void init(DXCore& core, const void* buffer, unsigned int sizeInBytes) {
//
//		D3D11_BUFFER_DESC bd;
//		bd.Usage = D3D11_USAGE_DYNAMIC;
//		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		bd.MiscFlags = 0;
//		D3D11_SUBRESOURCE_DATA data;
//		//bd.ByteWidth = sizeInBytes;
//		bd.ByteWidth = 16;
//		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		core.device->CreateBuffer(&bd, NULL, &constantBuffer);
//
//		D3D11_MAPPED_SUBRESOURCE mapped;
//		core.devicecontext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
//		memcpy(mapped.pData, buffer, sizeof(ConstantBuffer));
//		core.devicecontext->Unmap(constantBuffer, 0);
//	}
//
//	void update(DXCore& core, const void* buffer) {
//		D3D11_MAPPED_SUBRESOURCE mapped;
//		core.devicecontext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
//		memcpy(mapped.pData, buffer, sizeof(ConstantBuffer));
//		core.devicecontext->Unmap(constantBuffer, 0);
//	}
//
//	void apply(DXCore& core) {
//		core.devicecontext->PSSetConstantBuffers(0, 1, &constantBuffer);
//	}
//};