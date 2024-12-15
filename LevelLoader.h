#pragma once

#include "Mesh.h"
#include "Perspective.h"
#include "Animation.h"
#include <fstream>
#include <sstream>
#include "DXCore.h"

Vec3 str2Vec(std::string& s1, std::string& s2, std::string& s3) {
	return Vec3(std::stof(s1), std::stof(s2), std::stof(s3));
}

void loadGame(DXCore* core, std::string levelFile, ModelManager& models, ShaderManager& shaders, TextureManager& textures, Perspective& perspective) {
	std::ifstream save(levelFile);
	std::string fileLine;

	getline(save, fileLine);
	std::stringstream ss(fileLine);
	std::string val1;
	std::string val2;
	std::string val3;

	ss >> val1;
	Plane* plane = new Plane;
	plane->init(core, shaders, textures, val1);
	Object<Shape> planeInst(plane);
	models.addShapeInstance(&planeInst);
	delete plane;

	ss.clear();
	getline(save, fileLine);
	ss.str(fileLine);
	
	ss >> val1;
	ss >> val2;
	ss >> val3;
	Vec3 pos = str2Vec(val1, val2, val3);
	ss >> val1;
	ss >> val2;
	ss >> val3;
	Vec3 scale = str2Vec(val1, val2, val3);
	Cube* cube = new Cube;
	cube->init(core, shaders, textures);
	Object<Shape> cubeInst(cube, pos, scale);
	models.addShapeInstance(&cubeInst);
	delete cube;

	ss.clear();
	getline(save, fileLine);
	ss.str(fileLine);

	ss >> val1;
	int rad = stoi(val1);
	ss >> val1;
	ss >> val2;
	ss >> val3;
	pos = str2Vec(val1, val2, val3);
	Sphere* sphere = new Sphere;
	sphere->init(core, shaders, 10, 10, rad);
	Object<Shape> sphereInst(sphere, pos);
	models.addShapeInstance(&sphereInst);
	delete sphere;

	ss.clear();
	getline(save, fileLine);
	ss.str(fileLine);

	ss >> val1;
	int count = stoi(val1);
	ss >> val1;
	StaticModel* pine = new StaticModel;
	pine->init(core, shaders, textures, val1);

	for (int i = 0; i < count; i++) {
		ss >> val1;
		ss >> val2;
		ss >> val3;
		pos = str2Vec(val1, val2, val3);
		ss >> val1;
		ss >> val2;
		ss >> val3;
		scale = str2Vec(val1, val2, val3);
		Object<StaticModel> pine1(pine, pos, scale);
		models.addStaticInstance(&pine1);
	}
	delete pine;

	ss.clear();
	getline(save, fileLine);
	ss.str(fileLine);

	ss >> val1;
	count = stoi(val1);
	ss >> val1;
	AnimatedModel anim;
	anim.init(core, shaders, textures, val1);
	models.addAnimatedModel(anim);

	for (int i = 0; i < count; i++) {
		ss >> val1;
		ss >> val2;
		ss >> val3;
		pos = str2Vec(val1, val2, val3);
		ss >> val1;
		ss >> val2;
		ss >> val3;
		scale = str2Vec(val1, val2, val3);
		AnimatedModelInstance* dino = new AnimatedModelInstance;
		dino->init(models.animatedModels.back());
		ss >> val1;
		dino->update(val1, 0);
		Object<AnimatedModelInstance> objAnimated(dino, pos, scale);
		models.addAnimatedInstance(&objAnimated);
		delete dino;
	}

	save.close();
	ss.clear();
}