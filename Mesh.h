#pragma once
#include "mathlib.h"
#include "DXCore.h"
#include "ShaderManager.h"
#include "GEMLoader.h"

class Vertex {
public:
	Vec3 pos;
	Color color;
};

struct STATIC_VERTEX {
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
};

class Triangle {
public:
	Vertex vertices[3];
	ID3D11Buffer* vertexBuffer;

	Triangle(DXCore& dxcore) {
		vertices[0].pos = Vec3(0, 1.f, 0);
		vertices[0].color = Color(0, 1.0f, 0);
		vertices[1].pos = Vec3(-1.f, -1.f, 0);
		vertices[1].color = Color(1.0f, 0, 0);
		vertices[2].pos = Vec3(1.f, -1.f, 0);
		vertices[2].color = Color(0, 0, 1.0f);

		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA uploadData;
		bd.ByteWidth = sizeof(vertices);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		uploadData.pSysMem = vertices;
		uploadData.SysMemPitch = 0;
		uploadData.SysMemSlicePitch = 0;
		dxcore.device->CreateBuffer(&bd, &uploadData, &vertexBuffer);

	}

	void draw(DXCore& dxcore) {
		UINT offsets;
		offsets = 0;
		UINT strides = sizeof(Vertex);
		dxcore.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dxcore.devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		dxcore.devicecontext->Draw(3, 0);
	}

};

class Mesh {
public:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;

	int indicesSize;
	UINT strides;

	void init(DXCore& core, void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices) {
		D3D11_BUFFER_DESC bd;
		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;
		core.device->CreateBuffer(&bd, &data, &indexBuffer);
		bd.ByteWidth = vertexSizeInBytes * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		data.pSysMem = vertices;
		core.device->CreateBuffer(&bd, &data, &vertexBuffer);
		indicesSize = numIndices;
		strides = vertexSizeInBytes;
	}

	void init(DXCore& core, std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices) {
		init(core, &vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size());
	}

	void draw(DXCore& core) {
		UINT offsets = 0;
		core.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core.devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		core.devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		core.devicecontext->DrawIndexed(indicesSize, 0, 0);
	}

};

STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv) {
	STATIC_VERTEX v;
	v.pos = p;
	v.normal = n;
	ShadingFrame frame;
	//frame.fromVector(n);
	v.tangent = frame.t; // For now
	v.tu = tu;
	v.tv = tv;
	return v;
}

class Plane {
public:
	Mesh mesh;

	void init(DXCore& core, ShaderManager& shaders) {
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-15, 0, -15), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(15, 0, -15), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-15, 0, 15), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(15, 0, 15), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);
		mesh.init(core, vertices, indices);

		Shader shader;
		shaders.addShader("static", shader);
		shaders.shaders["static"].init(core, "vertexShader1.txt", "pixelShader1.txt");
	}

	void draw(DXCore& core, ShaderManager& shaders, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static", "staticMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static");
		mesh.draw(core);
	}
};

class Cube {
public:
	Mesh mesh;

	void init(DXCore& core, ShaderManager& shaders) {
		std::vector<STATIC_VERTEX> vertices;
		Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
		Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
		Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
		Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
		Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
		Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);

		vertices.push_back(addVertex(p0, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p3, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p5, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p4, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p4, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p0, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p3, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p1, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p2, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p3, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p4, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p0, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f));

		std::vector<unsigned int> indices;
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(0); indices.push_back(2); indices.push_back(3);
		indices.push_back(4); indices.push_back(5); indices.push_back(6);
		indices.push_back(4); indices.push_back(6); indices.push_back(7);
		indices.push_back(8); indices.push_back(9); indices.push_back(10);
		indices.push_back(8); indices.push_back(10); indices.push_back(11);
		indices.push_back(12); indices.push_back(13); indices.push_back(14);
		indices.push_back(12); indices.push_back(14); indices.push_back(15);
		indices.push_back(16); indices.push_back(17); indices.push_back(18);
		indices.push_back(16); indices.push_back(18); indices.push_back(19);
		indices.push_back(20); indices.push_back(21); indices.push_back(22);
		indices.push_back(20); indices.push_back(22); indices.push_back(23);

		mesh.init(core, vertices, indices);

		Shader shader;
		shaders.addShader("static", shader);
		shaders.shaders["static"].init(core, "vertexShader1.txt", "pixelShader1.txt");
	}

	void draw(DXCore& core, ShaderManager& shaders, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static", "staticMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static");
		mesh.draw(core);
	}
};

class Sphere {
public:
	Mesh mesh;

	void init(DXCore& core, ShaderManager& shaders, int rings, int segments, int radius) {
		std::vector<STATIC_VERTEX> vertices;

		for (int lat = 0; lat <= rings; lat++) {
			float theta = lat * M_PI / rings;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);

			for (int lon = 0; lon <= segments; lon++) {
				float phi = lon * 2.0f * M_PI / segments;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);
				Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta, radius * sinTheta * sinPhi);
				Vec3 normal = position.normalize();
				float tu = 1.0f - (float)lon / segments;
				float tv = 1.0f - (float)lat / rings;
				vertices.push_back(addVertex(position, normal, tu, tv));
			}
		}

		std::vector<unsigned int> indices;

		for (int lat = 0; lat < rings; lat++) {
			for (int lon = 0; lon < segments; lon++) {
				int current = lat * (segments + 1) + lon;
				int next = current + segments + 1;
				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);
				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}

		mesh.init(core, vertices, indices);

		Shader shader;
		shaders.addShader("static", shader);
		shaders.shaders["static"].init(core, "vertexShader1.txt", "pixelShader1.txt");
	}

	void draw(DXCore& core, ShaderManager& shaders, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static", "staticMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static");
		mesh.draw(core);
	}
};

class StaticModel {
public:
	std::vector<Mesh> meshes;

	void init(DXCore& core, ShaderManager& shaders, std::string modelFile) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(modelFile, gemmeshes);

		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh mesh;
			std::vector<STATIC_VERTEX> vertices;

			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
			}

			mesh.init(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);
		}

		Shader shader;
		shaders.addShader("static", shader);
		shaders.shaders["static"].init(core, "vertexShader1.txt", "pixelShader1.txt");
	}

	void draw(DXCore& core, ShaderManager& shaders, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static", "staticMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static");

		for (int i = 0; i < meshes.size(); i++) {
			meshes[i].draw(core);
		}

	}


};