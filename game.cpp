#include "mathlib.h"
#include "Window.h"
#include "DXCore.h"
#include "Mesh.h"
#include "Shaders.h"
//#include "ConstantBuffer.h"
#include "ShaderManager.h"
#include "GamesEngineeringBase.h"
#include "Perspective.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window* win = new Window;
	DXCore* dxcore = new DXCore;

	win->create(1024, 700, "Window");
	dxcore->init(win->width, win->height, win->hwnd, false);

	/*Triangle tri(*dxcore);
	Shader shader;*/
	ShaderManager shaders;
	TextureManager textures;

	Sampler sampler;
	sampler.init(dxcore);

	/*shaders->addShader("pulse", shader);
	shaders->shaders["pulse"].init(*dxcore, "Shaders/vs_basic.txt", "Shaders/ps_basic.txt", false);*/

	Plane plane;
	plane.init(dxcore, shaders);

	/*Cube cube;
	cube.init(dxcore, shaders);*/

	//Sphere sphere;
	//sphere.init(dxcore, shaders, 10, 10, 10);

	/*StaticModel tree;
	tree.init(dxcore, shaders, textures, "Resources/Models/acacia_003.gem");*/

	AnimatedModel* pump = new AnimatedModel;
	pump->init(dxcore, shaders, textures, "Resources/Models/Pump_Action_Shotgun.gem");

	AnimatedModelInstance shot;
	shot.init(pump);
	shot.update("Armature|01 Select", 0);

	AnimatedModel* trex = new AnimatedModel;
	trex->init(dxcore, shaders, textures, "Resources/Models/TRex.gem");

	StaticModel* pine = new StaticModel;
	pine->init(dxcore, shaders, textures, "Resources/Models/pine.gem");

	AnimatedModelInstance dino;
	dino.init(trex);
	dino.update("Run", 0);
	dino.changeAnimation("Run");

	/*AnimatedModel soldier;
	soldier.init(dxcore, shaders, textures, "Resources/Models/Soldier1.gem");

	AnimatedModelInstance guy;
	guy.init(&soldier);
	guy.update("toss grenade", 0);*/

	Vec3 pos(10, 25, 10);
	Persective cam(pos);
	cam.fwd = Vec3(0, 1, 0);

	float time = 0;

	int prevMouseX = win->mousex;
	int prevMouseY = win->mousey;

	//shaders.apply(dxcore, "pulse");

	/*ConstantBuffer buff;
	buff.init(dxcore, &constBuffCPU2, 48);
	buff.apply(dxcore);*/

	GamesEngineeringBase::Timer timer;
	timer.reset();

	float aspect = (float)win->width / (float)win->height;
	Matrix proj = Matrix::perspectiveProj(aspect, M_PI / 2.f, 0.05f, 100.f);

	Matrix worldMat;
	Matrix view;

	while (true) {
		dxcore->clear();

		win->processMessages();

		//win.clipMouseToWindow();

		float dt = timer.dt();
		time += dt;

		cam.gander(prevMouseX - win->mousex, win->mousey - prevMouseY);
		cam.meander(*win, dt);
		prevMouseX = win->mousex;
		prevMouseY = win->mousey;
		//win.updateMouse(win.width, win.height);

		Vec3 to = cam.pos + cam.fwd;
		view = Matrix::lookAt(cam.pos, to);
		view = view * proj;


		//Draw Things

		/*worldMat = Matrix::scaling(Vec3(100, 100, 100));
		sphere.draw(dxcore, shaders, &worldMat, &view, &time);*/

		worldMat.eye();
		plane.draw(dxcore, shaders, &worldMat, &view);

		/*
		worldMat = Matrix::translation(Vec3(0, 0, 5));
		sphere.draw(dxcore, shaders, &worldMat, &view, &time);

		worldMat = Matrix::translation(Vec3(5, 0, 0));
		sphere.draw(dxcore, shaders, &worldMat, &view, &time);

		worldMat = Matrix::scaling(Vec3(0.1f, 0.1f, 0.1f)) * Matrix::rotateY(sinf(time));
		tree.draw(dxcore, shaders, &worldMat, &view);*/

		worldMat = Matrix::scaling(Vec3(5, 5, 5)) *Matrix::translation(Vec3(20, 0, 0));
		dino.draw(dxcore, shaders, textures, &worldMat, &view, dt);

		worldMat = Matrix::scaling(Vec3(0.1f, 0.1f, 0.1f));
		pine->draw(dxcore, shaders, textures, &worldMat, &view, &time);

		//worldMat = Matrix::translation(Vec3(10.f, 0.f, 0.f));
		//guy.draw(dxcore, shaders, textures, &worldMat, &view, dt);

		//worldMat = Matrix::scaling(Vec3(0.1f, 0.1f, 0.1f)) * Matrix::rotateY(-cam.dir.phi) * Matrix::translation(cam.pos);
		//shot.draw(dxcore, shaders, textures, &worldMat, &view, dt);

		dxcore->present();
	}

	dxcore->close();
}