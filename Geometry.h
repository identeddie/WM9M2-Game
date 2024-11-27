#pragma once

#include "DXCore.h"
#include "mathlib.h"

struct STATIC_VERTEX {
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
};

class Plane {
public:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;

	int indicesSize;
	UINT strides;
};