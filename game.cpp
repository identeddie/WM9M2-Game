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

	Skybox sky;
	sky.init(dxcore, shaders, textures, 20, 20, 80);

	/*Cube cube;
	cube.init(dxcore, shaders);*/

	//Sphere sphere;
	//sphere.init(dxcore, shaders, 10, 10, 10);

	/*StaticModel tree;
	tree.init(dxcore, shaders, textures, "Resources/Models/acacia_003.gem");*/

	/*AnimatedModel* pump = new AnimatedModel;
	pump->init(dxcore, shaders, textures, "Resources/Models/Pump_Action_Shotgun.gem");

	AnimatedModelInstance* shot = new AnimatedModelInstance;
	shot->init(pump);
	shot->update("Armature|01 Select", 0);*/

	AnimatedModel* trex = new AnimatedModel;
	trex->init(dxcore, shaders, textures, "Resources/Models/TRex.gem");

	StaticModel* pine = new StaticModel;
	pine->init(dxcore, shaders, textures, "Resources/Models/pine.gem");

	AnimatedModelInstance* dino = new AnimatedModelInstance;
	dino->init(trex);
	dino->update("Run", 0);
	//dino.changeAnimation("Run");

	//AnimatedModel* soldier = new AnimatedModel;
	//soldier->init(dxcore, shaders, textures, "Resources/Models/Soldier1.gem");

	//AnimatedModelInstance guy;
	//guy.init(soldier);
	//guy.update("Idle (2)", 0);

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

	Shader* fsqShader = new Shader;
	fsqShader->init(dxcore, "Shaders/vs_fullscreenquad.txt", "Shaders/ps_fullscreenquad.txt", false);
	shaders.addShader("fsq", fsqShader);

	ShadowMap shadowmap;
	shadowmap.init(dxcore, 1028, 1028, shaders, textures);

	GBuffer gBuff;
	gBuff.init(dxcore, *win, textures, shadowmap);

	FullScreenQuad fsq;

	float aspect = (float)win->width / (float)win->height;
	Matrix proj = Matrix::perspectiveProj(aspect, M_PI / 2.f, 0.05f, 100.f);

	Matrix lightProj = Matrix::orthographicProj(400, 400, 0.1f, 200.f);
	Matrix lightView;

	Matrix worldMat;
	Matrix view;

	//Vec3 sunPos(20.f, 200.f, 0.f);
	Color sunColor(1.f, 1.f, 1.f);

	while (true) {
		dxcore->clear();

		win->processMessages();

		//win.clipMouseToWindow();

		Vec3 sunPos = Vec3(20.f * cosf(time), 40, 20.f * sinf(time));
		//Vec3 sunPos = Vec3(1.f, 0.2f, 0.1f);

		float dt = timer.dt();
		time += dt;

		lightProj = Matrix::orthographicProj(400, 400, 0.1f, 200);

		cam.gander(prevMouseX - win->mousex, win->mousey - prevMouseY);
		cam.meander(*win, dt);
		prevMouseX = win->mousex;
		prevMouseY = win->mousey;
		//win.updateMouse(win.width, win.height);
		 
		Vec3 to = cam.pos + cam.fwd;
		//to = Vec3(0, 30, 0);
		view = Matrix::lookAt(cam.pos, to);
		view = view * proj;

		to = Vec3(0.0);
		lightView = Matrix::lookAt(sunPos, to);
		lightView = lightView * lightProj;


		dino->update(dt);

		shadowmap.setupShadowPass(dxcore);

		worldMat.eye();
		plane.shadowDraw(dxcore, shaders, &worldMat, &lightView);
		worldMat = Matrix::scaling(Vec3(0.1f, 0.1f, 0.1f));
		pine->shadowDraw(dxcore, shaders, &worldMat, &lightView);
		//worldMat = Matrix::scaling(Vec3(5, 5, 5)) * Matrix::translation(Vec3(20, 0, 0));
		//dino->shadowDraw(dxcore, shaders, &worldMat, &lightView);

		//Set render target to get gbuff
		gBuff.setRenderTargets(dxcore);

		worldMat.eye();
		plane.draw(dxcore, shaders, &worldMat, &view);
		worldMat = Matrix::scaling(Vec3(0.1f, 0.1f, 0.1f));
		pine->draw(dxcore, shaders, textures, &worldMat, &view, &time);
		worldMat = Matrix::scaling(Vec3(5, 5, 5)) * Matrix::translation(Vec3(20, 0, 0));
		dino->draw(dxcore, shaders, textures, &worldMat, &view, dt);

		dxcore->renderToBackbuffer();

		//worldMat = Matrix::rotateX(M_PI) * Matrix::scaling(Vec3(200, 200, 200));
		//sky.draw(dxcore, shaders, textures, &worldMat, &view, &time);

		Matrix inverseView = view.invert();

		fsq.draw(dxcore, shaders, gBuff, &sunPos, &sunColor, &inverseView);

		//Draw Things

		/*worldMat = Matrix::scaling(Vec3(100, 100, 100));
		sphere.draw(dxcore, shaders, &worldMat, &view, &time);*/

		//worldMat.eye();
		//plane.draw(dxcore, shaders, &worldMat, &view);

		/*
		worldMat = Matrix::translation(Vec3(0, 0, 5));
		sphere.draw(dxcore, shaders, &worldMat, &view, &time);

		worldMat = Matrix::translation(Vec3(5, 0, 0));
		sphere.draw(dxcore, shaders, &worldMat, &view, &time);

		worldMat = Matrix::scaling(Vec3(0.1f, 0.1f, 0.1f)) * Matrix::rotateY(sinf(time));
		tree.draw(dxcore, shaders, &worldMat, &view);*/

		//worldMat = Matrix::scaling(Vec3(5, 5, 5)) * Matrix::translation(Vec3(10.f, 0.f, 0.f));
		//guy.draw(dxcore, shaders, textures, &worldMat, &view, dt);

		//worldMat = Matrix::scaling(Vec3(0.1f, 0.1f, 0.1f)) * Matrix::rotateY(-cam.dir.phi) * Matrix::translation(cam.pos);
		//shot->draw(dxcore, shaders, textures, &worldMat, &view, dt);

		dxcore->present();
	}

	dxcore->close();
}