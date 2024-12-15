#include "mathlib.h"
#include "Window.h"
#include "DXCore.h"
#include "Mesh.h"
#include "Shaders.h"
//#include "ConstantBuffer.h"
#include "ShaderManager.h"
#include "GamesEngineeringBase.h"
#include "Perspective.h"
#include "LevelLoader.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window* win = new Window;
	DXCore* dxcore = new DXCore;

	win->create(1024, 700, "Window");
	dxcore->init(win->width, win->height, win->hwnd, false);

	ShaderManager shaders;
	TextureManager textures;
	ModelManager models;
	Sampler sampler;
	sampler.init(dxcore); 
	Vec3 pos(10, 25, 10);
	Perspective cam(pos);

	Plane* plane = new Plane;
	plane->init(dxcore, shaders, textures, "Textures/grass.png");
	Object<Shape> planeInst(plane);
	models.addShapeInstance(&planeInst);

	Skybox sky;
	sky.init(dxcore, shaders, textures, 20, 20, 150);

	Cube* cube = new Cube;
	cube->init(dxcore, shaders, textures);
	Object<Shape> cubeInst(cube, Vec3(10, 0, 10), Vec3(3.f, 3.f, 3.f));
	models.addShapeInstance(&cubeInst);

	Sphere* sphere = new Sphere;
	sphere->init(dxcore, shaders, 10, 10, 10);
	Object<Shape> sphereInst(sphere, Vec3(0, 0, 0));
	models.addShapeInstance(&sphereInst);
	float radius = sphere->getRadius();

	AnimatedModel* trex = new AnimatedModel;
	trex->init(dxcore, shaders, textures, "Resources/Models/TRex.gem");

	StaticModel* pine = new StaticModel;
	pine->init(dxcore, shaders, textures, "Resources/Models/pine.gem");
	Object<StaticModel> pine1(pine, Vec3(0.f), Vec3(0.1f, 0.1f, 0.1f));
	models.addStaticInstance(&pine1);
	Object<StaticModel> pine2(pine, Vec3(-50.f, 0.f, 12.f), Vec3(0.1f, 0.1f, 0.1f));
	models.addStaticInstance(&pine2);
	Object<StaticModel> pine3(pine, Vec3(-85.f, 0.f, 15.f), Vec3(0.1f, 0.1f, 0.1f));
	models.addStaticInstance(&pine3);
	Object<StaticModel> pine4(pine, Vec3(85.f, 0.f, 40.f), Vec3(0.1f, 0.1f, 0.1f));
	models.addStaticInstance(&pine4);
	Object<StaticModel> pine5(pine, Vec3(70.f, 0.f, -30.f), Vec3(0.1f, 0.1f, 0.1f));
	models.addStaticInstance(&pine5);

	AnimatedModelInstance* dino = new AnimatedModelInstance;
	dino->init(trex);
	dino->update("Run", 0);
	Object<AnimatedModelInstance> dinoObj(dino, Vec3(50, 0, -50), Vec3(5, 5, 5));
	models.addAnimatedInstance(&dinoObj);

	GamesEngineeringBase::Timer timer;
	timer.reset();

	Shader* fsqShader = new Shader;
	fsqShader->init(dxcore, "Shaders/vs_fullscreenquad.txt", "Shaders/ps_fullscreenquad.txt", false);
	shaders.addShader("fsq", fsqShader);

	ShadowMap shadowmap;
	shadowmap.init(dxcore, 2048, 2048, shaders, textures);

	Reflection reflection;
	reflection.init(dxcore, 1024, textures, shaders);

	ReflectionPlane reflectionPlane;
	reflectionPlane.init(dxcore, shaders, textures, 50);

	GBuffer gBuff;
	gBuff.init(dxcore, *win, textures, shadowmap);

	FullScreenQuad fsq;

	//loadGame(dxcore, "loadfile.txt", models, shaders, textures, cam); //Does not work, unsolved data corruption

	float aspect = (float)win->width / (float)win->height;
	Matrix proj = Matrix::perspectiveProj(aspect, M_PI / 2.f, 0.05f, 250.f);
	Matrix view;
	Matrix lightProj = Matrix::orthographicProj(400, 400, -400.f, 400.f);
	Matrix lightView;

	Vec4 lightPos[4];
	lightPos[0] = Vec4(30.f, 10.f, -35.f);
	lightPos[1] = Vec4(40.f, 15.f, 40.f);
	lightPos[2] = Vec4(0.f, 60.f, 5.f);
	lightPos[3] = Vec4(0.f, 2.f, 10.f);

	Color lightColor[4];
	lightColor[0] = Color(0.8f, 0.6f, 0.6f);
	lightColor[1] = Color(0.6f, 0.5f, 1.f);
	lightColor[2] = Color(1.f, 1.f, 0.6f);
	lightColor[3] = Color(0.8f, 0.6f, 0.6f);

	Matrix worldMat;

	float time = 0;
	int prevMouseX = win->mousex;
	int prevMouseY = win->mousey;

	while (true) {
		dxcore->clear();

		win->processMessages();
		//win.clipMouseToWindow();

		Vec3 sunPos = Vec3(50.f * cosf(time), 40, 50.f * sinf(time));
		sunPos = Vec3(50.f, 40.f, 50.f);

		float dt = timer.dt();
		time += dt;

		if (win->keyPressed(' ')) {
			if (dino->instance.currentAnimation == "Run") {
				dinoObj.model->changeAnimation("roar");
			} else {
				dinoObj.model->changeAnimation("Run");
			}
		}

		cam.gander(prevMouseX - win->mousex, win->mousey - prevMouseY);
		Vec3 posChange = cam.meander(*win, dt);
		models.shapeInstances[2]->move(posChange);
		prevMouseX = win->mousex;
		prevMouseY = win->mousey;
		//win.updateMouse(win.width, win.height);
		 
		Vec3 from = models.shapeInstances[2]->pos + Vec3(0.f, cam.pos.y, 0.f);
		Vec3 to = from + cam.fwd;
		view = Matrix::lookAt(from, to);
		Matrix reflectionView = reflectionPlane.F * view;
		reflectionView = reflectionView * proj;
		view = view * proj;

		to = Vec3(0.f);
		from = Vec3(sunPos.x, sunPos.y, sunPos.z);
		lightView = Matrix::lookAt(from, to);
		lightView = lightView * lightProj;

		sunPos = sunPos.normalize();

		//Update animation for animated models
		models.updateAnimation(dt);
		//Handle collision between models
		models.handleCollision();

		//Reflection Pass - Render to reflection texture
		reflection.setupPass(dxcore);
		models.reflectionPass(dxcore, shaders, textures, &reflectionView);
		worldMat = Matrix::rotateX(M_PI);
		sky.reflectionPass(dxcore, shaders, textures, &worldMat, &reflectionView);

		shadowmap.setupShadowPass(dxcore);
		models.shadowDraw(dxcore, shaders, textures, &lightView); 

		//Main pass - Render to gbuffers
		gBuff.setRenderTargets(dxcore);
		models.draw(dxcore, shaders, textures, &view, &time, &dt);
		//planeInst.draw(dxcore, shaders, textures, &view, &time);
		Vec2 offsets(time / 10.f, time / 8.f);
		reflectionPlane.draw(dxcore, shaders, textures, &view, &offsets);

		//Final Pass - Lighting and Skybox
		dxcore->renderToBackbuffer();
		worldMat = Matrix::rotateX(M_PI);
		sky.draw(dxcore, shaders, textures, &worldMat, &view, &time);
		fsq.draw(dxcore, shaders, gBuff, &sunPos, lightPos, lightColor, &lightView);

		//worldMat = Matrix::scaling(Vec3(5, 5, 5)) * Matrix::translation(Vec3(10.f, 0.f, 0.f));
		//guy.draw(dxcore, shaders, textures, &worldMat, &view, dt);
		//worldMat = Matrix::scaling(Vec3(0.1f, 0.1f, 0.1f)) * Matrix::rotateY(-cam.dir.phi) * Matrix::translation(cam.pos);
		//shot->draw(dxcore, shaders, textures, &worldMat, &view, dt);

		dxcore->present();
	}

	dxcore->close();
}