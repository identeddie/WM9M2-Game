#pragma once
#include "mathlib.h"
#include "DXCore.h"
#include "ShaderManager.h"
#include "GEMLoader.h"
#include "Animation.h"
#include "Collisions.h"

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

struct ANIMATED_VERTEX {
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
	unsigned int bonesIDs[4];
	float boneWeights[4];
};

struct STATIC_SHADOW_VERTEX {
	Vec3 pos;
};

struct ANIMATED_SHADOW_VERTEX {
	Vec3 pos;
	unsigned int bonesIDs[4];
	float boneWeights[4];
};

class Triangle {
public:
	Vertex vertices[3];
	ID3D11Buffer* vertexBuffer;

	Triangle(DXCore* dxcore) {
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
		dxcore->device->CreateBuffer(&bd, &uploadData, &vertexBuffer);

	}

	void draw(DXCore* dxcore) {
		UINT offsets;
		offsets = 0;
		UINT strides = sizeof(Vertex);
		dxcore->devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dxcore->devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		dxcore->devicecontext->Draw(3, 0);
	}
};

class Mesh {
public:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;

	int indicesSize;
	UINT strides;

	void init(DXCore* core, void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices) {
		D3D11_BUFFER_DESC bd;
		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;
		core->device->CreateBuffer(&bd, &data, &indexBuffer);
		bd.ByteWidth = vertexSizeInBytes * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		data.pSysMem = vertices;
		core->device->CreateBuffer(&bd, &data, &vertexBuffer);
		indicesSize = numIndices;
		strides = vertexSizeInBytes;
	}

	void init(DXCore* core, std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices) {
		init(core, &vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size());
	}

	void init(DXCore* core, std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices) {
		init(core, &vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size());
	}

	void draw(DXCore* core) {
		UINT offsets = 0;
		core->devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core->devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		core->devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		core->devicecontext->DrawIndexed(indicesSize, 0, 0);
	}

};

STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv) {
	STATIC_VERTEX v;
	v.pos = p;
	v.normal = n;
	ShadingFrame frame(n);
	//frame.T();
	//frame.fromVector(n);
	v.tangent = frame.t; // For now
	v.tu = tu;
	v.tv = tv;
	return v;
}

class Shape {
public:
	Mesh mesh;
	std::vector<AABB> boundingBoxes;
	Color color;
	std::string texPath;

	float getRadius() {
		return (boundingBoxes[0].max.x - boundingBoxes[0].min.x) / 2.f;
	}

	virtual void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj, float* t) {};
	virtual void shadowDraw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {};
	virtual void reflectionPass(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {};
	virtual Shape* copy() const = 0;
};

class Plane : public Shape {
public:
	Plane* copy() const override {
		return new Plane(*this);
	}

	void init(DXCore* core, ShaderManager& shaders, TextureManager& textures, std::string texturePath) {
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-150, 0, -150), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(150, 0, -150), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-150, 0, 150), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(150, 0, 150), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);
		mesh.init(core, vertices, indices);

		AABB boundingBox;

		for (int i = 0; i < vertices.size(); i++) {
			boundingBox.extend(vertices[i].pos);
		}

		boundingBoxes.push_back(boundingBox);

		color = Color(0.f, 0.7f, 0.f);

		Shader* shader = new Shader;
		shader->init(core, "Shaders/vs_static.txt", "Shaders/ps_static.txt", 0);
		shaders.addShader("static", shader);

		Shader* tilingShader = new Shader;
		tilingShader->init(core, "Shaders/vs_static.txt", "Shaders/ps_tex_tiling.txt", 0);
		shaders.addShader("static_tex_tiling", tilingShader);

		texPath = texturePath;
		Texture* texture = new Texture;
		texture->load(core, texturePath);
		textures.addTexture(texturePath, texture);

		Vec3 normal = vertices[0].normal.normalize();
		float d = -dot(vertices[0].pos, normal);
	}

	void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj, float* t) {
		shaders.updateConstantVS("static_tex_tiling", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_tex_tiling", "staticMeshBuffer", "VP", viewProj);
		//shaders.updateConstantPS("static_tex", "colorBuff", "Color", &color);
		shaders.apply(core, "static_tex_tiling");

		shaders.shaders["static_tex_tiling"]->bindTexturePS(core, "tex", textures.find(texPath));
		mesh.draw(core);
	}

	void shadowDraw(DXCore* core, ShaderManager& shaders, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static_shadow", "staticShadowMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_shadow", "staticShadowMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static_shadow");

		mesh.draw(core);
	}

	void reflectionPass(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static_reflection");

		mesh.draw(core);
	}
};

class Cube : public Shape {
public:
	Cube* copy() const override {
		return new Cube(*this);
	}

	void init(DXCore* core, ShaderManager& shaders, TextureManager& textures) {
		std::vector<STATIC_VERTEX> vertices;
		Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
		Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
		Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
		Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
		Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
		Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);

		vertices.push_back(addVertex(p0 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p1 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p2 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p3 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p5 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p4 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p7 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p6 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p4 + Vec3(0.f, 1.f, 0.f), Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p0 + Vec3(0.f, 1.f, 0.f), Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p3 + Vec3(0.f, 1.f, 0.f), Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7 + Vec3(0.f, 1.f, 0.f), Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p1 + Vec3(0.f, 1.f, 0.f), Vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5 + Vec3(0.f, 1.f, 0.f), Vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6 + Vec3(0.f, 1.f, 0.f), Vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p2 + Vec3(0.f, 1.f, 0.f), Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p3 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p2 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p4 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p1 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p0 + Vec3(0.f, 1.f, 0.f), Vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f));

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

		AABB boundingBox;

		for (int i = 0; i < vertices.size(); i++) {
			boundingBox.extend(vertices[i].pos);
		}

		boundingBoxes.push_back(boundingBox);

		mesh.init(core, vertices, indices);

		color = Color(0.f, 0.f, 0.6f);

		Shader* shader = new Shader;
		shader->init(core, "Shaders/vs_static.txt", "Shaders/ps_textured.txt", 0);
		shaders.addShader("static_tex", shader);

		Texture* brickAlbedo = new Texture;
		brickAlbedo->load(core, "Textures/rounded-brick1-albedo.png");
		textures.addTexture("brick_albedo", brickAlbedo);

		Texture* brickNormal = new Texture;
		brickNormal->load(core, "Textures/rounded-brick1-normal.png");
		textures.addTexture("brick_normal", brickNormal);
	}

	void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj, float* t) {
		shaders.updateConstantVS("static_tex", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_tex", "staticMeshBuffer", "VP", viewProj);
		shaders.updateConstantPS("static_tex", "colorBuff", "Color", &color);
		shaders.apply(core, "static_tex");

		shaders.shaders["static_tex"]->bindTexturePS(core, "tex", textures.find("brick_albedo"));
		shaders.shaders["static_tex"]->bindTexturePS(core, "tex_norm", textures.find("brick_normal"));
		mesh.draw(core);
	}

	void shadowDraw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static_shadow", "staticShadowMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_shadow", "staticShadowMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static_shadow");

		shaders.shaders["static_shadow"]->bindTexturePS(core, "tex", textures.find("brick_albedo"));
		mesh.draw(core);
	}

	void reflectionPass(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static_reflection");

		shaders.shaders["static_reflection"]->bindTexturePS(core, "tex", textures.find("brick_albedo"));
		mesh.draw(core);
	}
};

class Sphere : public Shape {
public:
	Sphere* copy() const override {
		return new Sphere(*this);
	}

	void init(DXCore* core, ShaderManager& shaders, int rings, int segments, int radius) {
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
				Vec3 normal = position;
				normal.normalize();
				float tu = 1.0f - (float)lon / segments;
				float tv = 1.0f - (float)lat / rings;
				vertices.push_back(addVertex(position + Vec3(0.f, radius, 0.f), normal, tu, tv));
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

		AABB boundingBox;

		for (int i = 0; i < vertices.size(); i++) {
			boundingBox.extend(vertices[i].pos);
		}

		boundingBoxes.push_back(boundingBox);

		color = Color(0.7f, 0.f, 0.f);

		Shader* shader = new Shader;
		shader->init(core, "Shaders/vs_static.txt", "Shaders/ps_static.txt", 0);
		shaders.addShader("static_pulse", shader);
	}

	void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj, float* time) {
		shaders.updateConstantVS("static", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static", "staticMeshBuffer", "VP", viewProj);
		shaders.updateConstantPS("static", "colorBuff", "Color", &color);
		shaders.apply(core, "static");
		mesh.draw(core);
	}

	void shadowDraw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static_shadow", "staticShadowMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_shadow", "staticShadowMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static_shadow");
		mesh.draw(core);
	}

	void reflectionPass(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static_reflection");

		mesh.draw(core);
	}
};

class Skybox {
public:
	Mesh mesh;

	void init(DXCore* core, ShaderManager& shaders, TextureManager& textures, int rings, int segments, int radius) {
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
				Vec3 normal = position;
				normal.normalize();
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

		Shader* shader = new Shader;
		shader->init(core, "Shaders/vs_static.txt", "Shaders/ps_textured_sky.txt", false);
		shaders.addShader("static_sky", shader);

		Texture* texture = new Texture;
		texture->load(core, "Textures/sky.png");
		textures.addTexture("Textures/sky.png", texture);
	}

	void reflectionPass(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static_reflection");

		shaders.shaders["static_reflection"]->bindTexturePS(core, "tex", textures.find("Textures/sky.png"));
		mesh.draw(core);
	}

	void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj, float* time) {
		shaders.updateConstantVS("static_sky", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_sky", "staticMeshBuffer", "VP", viewProj);
		shaders.updateConstantPS("static_sky", "timeBuff", "time", time);
		shaders.apply(core, "static_sky");

		shaders.shaders["static_sky"]->bindTexturePS(core, "tex", textures.find("Textures/sky.png"));
		mesh.draw(core);
	}
};

class StaticModel {
public:
	std::vector<Mesh> meshes;
	std::vector<std::string> textureFilenames;
	std::vector<std::string> normalTextureFilenames;
	std::vector<AABB> boundingBoxes;

	StaticModel* copy() const {
		return new StaticModel(*this);
	}

	void init(DXCore* core, ShaderManager& shaders, TextureManager& textures, std::string modelFile) {
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

			textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
			normalTextureFilenames.push_back(gemmeshes[i].material.find("normals").getValue());
			mesh.init(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);

			AABB boundingBox;

			for (int i = 0; i < vertices.size(); i++) {
				boundingBox.extend(vertices[i].pos);
			}

			boundingBoxes.push_back(boundingBox);
			boundingBoxes[i].reduceXY(0.5f, 0.5f); //make tree base bounding box smaller
		}

		Shader* shader = new Shader;
		shader->init(core, "Shaders/vs_static_movement.txt", "Shaders/ps_textured_nomapping.txt", 0);
		shaders.addShader("static_tex_movement", shader);

		for (int i = 0; i < textureFilenames.size(); i++) {
			Texture* texture = new Texture;
			texture->load(core, textureFilenames[i]);
			textures.addTexture(textureFilenames[i], texture);
		}

		for (int i = 0; i < normalTextureFilenames.size(); i++) {
			Texture* texture = new Texture;
			texture->load(core, normalTextureFilenames[i]);
			textures.addTexture(normalTextureFilenames[i], texture);
		}
	}

	void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj, float* time) {
		shaders.updateConstantVS("static_tex_movement", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_tex_movement", "staticMeshBuffer", "VP", viewProj);
		shaders.updateConstantVS("static_tex_movement", "staticMeshBuffer", "time", time);
		shaders.apply(core, "static_tex_movement");

		for (int i = 0; i < meshes.size(); i++) {
			shaders.shaders["static_tex_movement"]->bindTextureVS(core, "tex", textures.find(textureFilenames[i]));

			ID3D11ShaderResourceView* views[2];
			views[0] = textures.find(textureFilenames[i]);
			views[1] = textures.find(normalTextureFilenames[i]);
			core->devicecontext->PSSetShaderResources(0, 2, views);

			//shaders.shaders["static_tex"]->bindTexturePS(core, "tex", textures.find(textureFilenames[i]));
			//shaders.shaders["static_tex"]->bindTexturePS(core, "tex_norm", textures.find(normalTextureFilenames[i]));
			meshes[i].draw(core);
		}
	}

	void shadowDraw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static_shadow", "staticShadowMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_shadow", "staticShadowMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static_shadow");

		for (int i = 0; i < meshes.size(); i++) {
			shaders.shaders["static_shadow"]->bindTexturePS(core, "tex", textures.find(textureFilenames[i]));
			meshes[i].draw(core);
		}
	}

	void reflectionPass(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("static_reflection", "staticMeshBuffer", "VP", viewProj);
		shaders.apply(core, "static_reflection");

		for (int i = 0; i < meshes.size(); i++) {
			shaders.shaders["static_tex"]->bindTextureVS(core, "tex", textures.find(textureFilenames[i]));

			ID3D11ShaderResourceView* views[2];
			views[0] = textures.find(textureFilenames[i]);
			views[1] = textures.find(normalTextureFilenames[i]);
			core->devicecontext->PSSetShaderResources(0, 2, views);

			//shaders.shaders["static_tex"]->bindTexturePS(core, "tex", textures.find(textureFilenames[i]));
			//shaders.shaders["static_tex"]->bindTexturePS(core, "tex_norm", textures.find(normalTextureFilenames[i]));
			meshes[i].draw(core);
		}
	}

};

class AnimatedModel {
public:
	std::vector<Mesh> meshes;
	Animation animation;
	std::vector<std::string> textureFilenames;
	std::vector<std::string> normalTextureFilenames;
	std::vector<AABB> boundingBoxes;

	void init(DXCore* core, ShaderManager& shaders, TextureManager& textures, std::string modelFile) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		loader.load(modelFile, gemmeshes, gemanimation);

		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh mesh;
			std::vector<ANIMATED_VERTEX> vertices;

			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++) {
				ANIMATED_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
				vertices.push_back(v);
			}

			textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
			normalTextureFilenames.push_back(gemmeshes[i].material.find("normals").getValue());
			mesh.init(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);

			AABB boundingBox;

			for (int i = 0; i < vertices.size(); i++) {
				boundingBox.extend(vertices[i].pos);
			}

			boundingBoxes.push_back(boundingBox);
		}

		for (int i = 0; i < gemanimation.bones.size(); i++) {
			Bone bone;
			bone.name = gemanimation.bones[i].name;
			memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
			bone.parentIndex = gemanimation.bones[i].parentIndex;
			animation.skeleton.bones.push_back(bone);
		}

		for (int i = 0; i < gemanimation.animations.size(); i++) {
			std::string name = gemanimation.animations[i].name;
			AnimationSequence aseq;
			aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;

			for (int n = 0; n < gemanimation.animations[i].frames.size(); n++) {
				AnimationFrame frame;

				for (int index = 0; index < gemanimation.animations[i].frames[n].positions.size(); index++) {
					Vec3 p;
					Quaternion q;
					Vec3 s;

					memcpy(&p, &gemanimation.animations[i].frames[n].positions[index], sizeof(Vec3));
					frame.positions.push_back(p);
					memcpy(&q, &gemanimation.animations[i].frames[n].rotations[index], sizeof(Quaternion));
					frame.rotations.push_back(q);
					memcpy(&s, &gemanimation.animations[i].frames[n].scales[index], sizeof(Vec3));
					frame.scales.push_back(s);
				}

				aseq.frames.push_back(frame);
			}

			animation.animations.insert({ name, aseq });
		}

		for (int i = 0; i < textureFilenames.size(); i++) {
			Texture* texture = new Texture;
			texture->load(core, textureFilenames[i]);
			textures.addTexture(textureFilenames[i], texture);
		}

		for (int i = 0; i < normalTextureFilenames.size(); i++) {
			Texture* texture = new Texture;
			texture->load(core, normalTextureFilenames[i]);
			textures.addTexture(normalTextureFilenames[i], texture);
		}

		Shader* shader = new Shader;
		shader->init(core, "Shaders/vs_animated.txt", "Shaders/ps_textured.txt", 1);
		shaders.addShader("animated", shader);
	}
};

class AnimatedModelInstance {
public:
	AnimatedModel* model;
	AnimationInstance instance;
	std::vector<AABB> boundingBoxes;

	//Vec3 pos;
	//Vec3 scale;

	AnimatedModelInstance* copy() const {
		return new AnimatedModelInstance(*this);
	}

	void init(AnimatedModel* _model) {
		model = _model;
		instance.init(&model->animation);
		boundingBoxes = model->boundingBoxes;
	}

	void changeAnimation(std::string animation) {
 		instance.update(animation, 0);
	}

	void update(std::string animation, float dt) {
		instance.update(animation, dt);
	}

	void update(float dt) {
		instance.update(instance.currentAnimation, dt);
	}

	void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj, float* dt) {
		//instance.update(instance.currentAnimation, dt);

		shaders.updateConstantVS("animated", "animatedMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("animated", "animatedMeshBuffer", "VP", viewProj);
		shaders.updateConstantVS("animated", "animatedMeshBuffer", "bones", instance.matrices);
		shaders.apply(core, "animated");

		for (int i = 0; i < model->meshes.size(); i++) {
			//shaders.shaders["animated"]->bindTexturePS(core, "tex", textures.find(model->textureFilenames[i]));
			//shaders.shaders["animated"]->bindTexturePS(core, "tex_norm", textures.find(model->normalTextureFilenames[i]));

			ID3D11ShaderResourceView* views[2];
			views[0] = textures.find(model->textureFilenames[i]);
			views[1] = textures.find(model->normalTextureFilenames[i]);

			core->devicecontext->PSSetShaderResources(0, 2, views);
			model->meshes[i].draw(core);
		}
	}

	void reflectionPass(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("animated_reflection", "animatedMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("animated_reflection", "animatedMeshBuffer", "VP", viewProj);
		shaders.updateConstantVS("animated_reflection", "animatedMeshBuffer", "bones", instance.matrices);
		shaders.apply(core, "animated_reflection");

		for (int i = 0; i < model->meshes.size(); i++) {
			shaders.shaders["animated_reflection"]->bindTextureVS(core, "tex", textures.find(model->textureFilenames[i]));

			ID3D11ShaderResourceView* views[2];
			views[0] = textures.find(model->textureFilenames[i]);
			views[1] = textures.find(model->normalTextureFilenames[i]);
			core->devicecontext->PSSetShaderResources(0, 2, views);

			//shaders.shaders["static_tex"]->bindTexturePS(core, "tex", textures.find(textureFilenames[i]));
			//shaders.shaders["static_tex"]->bindTexturePS(core, "tex_norm", textures.find(normalTextureFilenames[i]));
			model->meshes[i].draw(core);
		}
	}

	void shadowDraw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* worldMat, Matrix* viewProj) {
		shaders.updateConstantVS("animated_shadow", "animatedShadowMeshBuffer", "W", worldMat);
		shaders.updateConstantVS("animated_shadow", "animatedShadowMeshBuffer", "VP", viewProj);
		shaders.updateConstantVS("animated_shadow", "animatedShadowMeshBuffer", "bones", instance.matrices);
		shaders.apply(core, "animated_shadow");
		//core->devicecontext->PSSetShader(NULL, NULL, 0);

		for (int i = 0; i < model->meshes.size(); i++) {
			shaders.shaders["animated_shadow"]->bindTexturePS(core, "tex", textures.find(model->textureFilenames[i]));
			model->meshes[i].draw(core);
		}
	}
};

class ReflectionPlane : public Plane {
public:
	Matrix F;
	Vec3 pos;
	Vec4 planarConstants;

	void init(DXCore* core, ShaderManager& shaders, TextureManager& textures, int width) {
		pos = Vec3(40.f, 0.1f, 40.f);

		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-width / 2, 0, -width / 2), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(width / 2, 0, -width / 2), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-width / 2, 0, width / 2), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(width / 2, 0, width / 2), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);
		mesh.init(core, vertices, indices);

		AABB boundingBox;

		for (int i = 0; i < vertices.size(); i++) {
			boundingBox.extend(vertices[i].pos);
		}

		boundingBoxes.push_back(boundingBox);

		color = Color(0.f, 0.f, 0.8f);

		Shader* shader = new Shader;
		shader->init(core, "Shaders/vs_static.txt", "Shaders/ps_reflectionplane.txt", 0);
		shaders.addShader("reflectionPlane", shader);

		Vec3 normal = vertices[0].normal.normalize();
		float d = -dot(vertices[0].pos, normal);

		planarConstants = Vec4(normal.x, normal.y, normal.z, d);
		Vec4 u = planarConstants;

		Matrix T;
		T[0] = 1 - 2 * sq(u.x);
		T[1] = -2 * u.x * u.y;
		T[2] = -2 * u.x * u.z;
		T[4] = T[1];
		T[5] = 1 - 2 * sq(u.y);
		T[6] = -2 * u.y * u.z;
		T[8] = T[2];
		T[9] = T[6];
		T[10] = 1 - 2 * sq(u.z);
		T[12] = -2 * u.x * u.w;
		T[13] = -2 * u.y * u.w;
		T[14] = -2 * u.z * u.w;

		F = T;

		Texture* waterNormal = new Texture;
		waterNormal->load(core, "Textures/watertex.gif");
		textures.addTexture("water", waterNormal);

		Texture* waterUnder = new Texture;
		waterUnder->load(core, "Textures/underwater.png");
		textures.addTexture("underwater", waterUnder);
	}

	void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* viewProj, Vec2* offsets) {
		Matrix worldMat = Matrix::translation(pos);
		shaders.updateConstantVS("reflectionPlane", "staticMeshBuffer", "W", &worldMat);
		shaders.updateConstantVS("reflectionPlane", "staticMeshBuffer", "VP", viewProj);
		shaders.updateConstantPS("reflectionPlane", "uvOffsetBuff", "offsets", offsets);
		shaders.apply(core, "reflectionPlane");

		shaders.shaders["reflectionPlane"]->bindTexturePS(core, "tex", textures.find("reflection"));
		shaders.shaders["reflectionPlane"]->bindTexturePS(core, "tex_norm", textures.find("water"));
		shaders.shaders["reflectionPlane"]->bindTexturePS(core, "tex_under", textures.find("underwater"));
		//core->devicecontext->PSSetShaderResources(0, 1, textures.find("reflection"));
		mesh.draw(core);
	}
};

class FullScreenQuad {
public:
	void draw(DXCore* core, ShaderManager& shaders, GBuffer& gBuffer, Vec3* sunPos, Vec4* lightPos, Color* lightColor, Matrix* sunVP) {
		shaders.updateConstantPS("fsq", "lightingBuffer", "sunPos", sunPos);
		shaders.updateConstantPS("fsq", "lightingBuffer", "lightPos", lightPos);
		shaders.updateConstantPS("fsq", "lightingBuffer", "lightColor", lightColor);
		shaders.updateConstantPS("fsq", "lightingBuffer", "sunVP", sunVP);
		shaders.apply(core, "fsq");
		core->devicecontext->PSSetShaderResources(0, 4, gBuffer.shaderResourceViews);
		core->devicecontext->IASetInputLayout(NULL); // Input layout is not used for fullscreen triangles
		core->devicecontext->Draw(3, 0);
	}
};

template <typename T>
class Object {
public:
	T* model;
	Vec3 pos;
	Vec3 scale;

	Object(T* _model) {
		model = _model->copy();
		pos = Vec3(0.f, 0.f, 0.f);
		scale = Vec3(1.f, 1.f, 1.f);
	}

	Object(T* _model, const Vec3& _pos) {
		model = _model->copy();
		pos = _pos;
		scale = Vec3(1.f, 1.f, 1.f);
	}

	Object(T* _model, const Vec3& _pos, const Vec3& _scale) {
		model = _model->copy();
		pos = _pos;
		scale = _scale;

		for (int i = 0; i < model->boundingBoxes.size(); i++) {
			model->boundingBoxes[i].max = Matrix::scaling(scale).mulPoint(model->boundingBoxes[i].max);
			model->boundingBoxes[i].min = Matrix::scaling(scale).mulPoint(model->boundingBoxes[i].min);
		}
	}

	void move(const Vec3& dx) {
		pos += dx;
		
		/*for (int i = 0; i < model->boundingBoxes.size(); i++) {
			model->boundingBoxes[i] += dx;
		}*/
	}

	void resize(float k) {
		scale *= k;
	}

	void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* viewProj, float* time) {
		Matrix worldMat = Matrix::scaling(scale) * Matrix::translation(pos);
		model->draw(core, shaders, textures, &worldMat, viewProj, time);
	}

	void shadowDraw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* viewProj) {
		Matrix worldMat = Matrix::scaling(scale) * Matrix::translation(pos);
		model->shadowDraw(core, shaders, textures, &worldMat, viewProj);
	}

	void reflectionPass(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* viewProj) {
		Matrix worldMat = Matrix::scaling(scale) * Matrix::translation(pos);
		model->reflectionPass(core, shaders, textures, &worldMat, viewProj);
	}
};

template <typename T1, typename T2>
void checkCollision(Object<T1>* obj1, Object<T2>* obj2) {
	for (int i = 0; i < obj1->model->boundingBoxes.size(); i++) {
		AABB bb1 = obj1->model->boundingBoxes[i] + obj1->pos;
		for (int j = 0; j < obj2->model->boundingBoxes.size(); j++) {
			AABB bb2 = obj2->model->boundingBoxes[j] + obj2->pos;
			Vec3 norm = bb1.checkCollision(bb2);

			if (norm.x != 0.f && norm.y != 0.f && norm.z != 0.f) {
				Vec3 normAbs(abs(norm.x), abs(norm.y), abs(norm.z));
				float minDiff = normAbs.Min();

				if (minDiff == abs(norm.x)) {
					norm = Vec3(norm.x, 0.f, 0.f);
				} else /*if (minDiff == abs(norm.z))*/ {
					norm = Vec3(0.f, 0.f, norm.z);
				}
				// else {
				//	//norm = Vec3(0.f, 0.f, norm.z);
				//	norm = Vec3(0.f);
				//}

				obj1->move(norm);
			}
		}
	}
}

class ModelManager {
public:
	std::vector<Object<StaticModel>*> staticInstances;
	std::vector<Object<AnimatedModelInstance>*> animatedInstances;
	std::vector<Object<Shape>*> shapeInstances;
	std::vector<AnimatedModel*> animatedModels;

	void addAnimatedModel(AnimatedModel _model) {
		AnimatedModel* model = new AnimatedModel(_model);
		animatedModels.push_back(model);
	}

	void addStaticInstance(Object<StaticModel>* inst) {
		staticInstances.push_back(inst);
	}

	void addAnimatedInstance(Object<AnimatedModelInstance>* inst) {
		animatedInstances.push_back(inst);
	}

	void addShapeInstance(Object<Shape>* inst) {
		shapeInstances.push_back(inst);
	}

	void updateAnimation(float dt) {
		for (int i = 0; i < animatedInstances.size(); i++) {
			animatedInstances[i]->model->update(dt);
		}
	}

	void handleCollision() {
		for (int i = 0; i < shapeInstances.size(); i++) {
			if (i < shapeInstances.size() - 1) {
				checkCollision(shapeInstances[i], shapeInstances[i + 1]);
			}

			for (int j = 0; j < staticInstances.size(); j++) {
				checkCollision(shapeInstances[i], staticInstances[j]);
			}

			for (int j = 0; j < animatedInstances.size(); j++) {
				checkCollision(shapeInstances[i], animatedInstances[j]);
			}
		}
	}

	void draw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* viewProj, float* time, float* dt) {
		for (int i = 0; i < staticInstances.size(); i++) {
			staticInstances[i]->draw(core, shaders, textures, viewProj, time);
		}

		for (int i = 0; i < animatedInstances.size(); i++) {
			animatedInstances[i]->draw(core, shaders, textures, viewProj, dt);
		}

		for (int i = 0; i < shapeInstances.size(); i++) {
			shapeInstances[i]->draw(core, shaders, textures, viewProj, time);
		}
	}

	void shadowDraw(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* viewProj) {
		for (int i = 0; i < staticInstances.size(); i++) {
			staticInstances[i]->shadowDraw(core, shaders, textures, viewProj);
		}

		for (int i = 0; i < animatedInstances.size(); i++) {
			animatedInstances[i]->shadowDraw(core, shaders, textures, viewProj);
		}

		for (int i = 0; i < shapeInstances.size(); i++) {
			shapeInstances[i]->shadowDraw(core, shaders, textures, viewProj);
		}
	}

	void reflectionPass(DXCore* core, ShaderManager& shaders, TextureManager& textures, Matrix* viewProj) {
		for (int i = 0; i < staticInstances.size(); i++) {
			staticInstances[i]->reflectionPass(core, shaders, textures, viewProj);
		}

		for (int i = 0; i < animatedInstances.size(); i++) {
			animatedInstances[i]->reflectionPass(core, shaders, textures, viewProj);
		}

		for (int i = 0; i < shapeInstances.size(); i++) {
			shapeInstances[i]->reflectionPass(core, shaders, textures, viewProj);
		}
	}
};