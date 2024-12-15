#pragma once
#include "mathlib.h"
#include "Window.h"

class Perspective {
public:
	Vec3 pos; //Position of perspective

	SphericalCoord dir; //Direction of perspective

	Vec3 fwd;
	Vec3 up;
	Vec3 right;

	float speed;
	float sensitivity;

	void GramSchmit() {
		fwd = dir.toCartesianCam();
		fwd.normalize();

		up = Vec3(0.f, 1.f, 0.f);

		if (fwd.y == up.y || fwd.y == -up.y) { //If colinear, change to (1, 0, 0)
			up.x = 1.f;
			up.y = 0.f;
		}

		right = up.cross(fwd);
		right.normalize();

		up = fwd.cross(right);
		up.normalize();
	}

	Perspective() {
		pos = Vec3(0.f);
		dir = SphericalCoord(M_PI / 2.f, 0.f);
		fwd = dir.toCartesianCam();
		up = Vec3(0.f, 1.f, 0.f);

		speed = 1.f;
		sensitivity = 1.f;
	}

	Perspective(Vec3& _pos) {
		pos = _pos;
		dir = SphericalCoord(M_PI / 2.f, M_PI);
		up = Vec3(0.f, 1.f, 0.f);

		speed = 10.f;
		sensitivity = 0.1f;

		GramSchmit();
	}

	void gander(float dx, float dy) {
		dx *= sensitivity;
		dy *= sensitivity;

		if (dx != 0.f || dy != 0.f) {
			dir.theta += dy;
			dir.phi += dx;

			if (dir.theta >= M_PI - 0.1f) {
				dir.theta = M_PI - 0.1f;
			} else if (dir.theta < 0.1f) {
				dir.theta = 0.1f;
			}

			if (dir.phi >= M_PI * 2) {
				dir.phi -= M_PI * 2;
			} else if (dir.phi < 0.f) {
				dir.phi += M_PI * 2;
			}

			GramSchmit();
		}
	}

	Vec3 meander(Window& win, float dt) {
		Vec3 posChange;

		if (win.keyPressed('W')) {
			posChange.x += fwd.x;
			posChange.z += fwd.z;
		}

		if (win.keyPressed('S')) {
			posChange.x -= fwd.x;
			posChange.z -= fwd.z;
		}

		if (win.keyPressed('A')) {
			posChange.x += right.x;
			posChange.z += right.z;
		}

		if (win.keyPressed('D')) {
			posChange.x -= right.x;
			posChange.z -= right.z;
		}

		float mag = posChange.mag();

		if (mag != 0.f) {
			posChange.normalize();
			float k = speed * dt;
			posChange = posChange * k; //Normalize movement vector, cap movement at sprite's speed value
			pos += posChange;
			return posChange;
		} else {
			return Vec3(0.f);
		}
	}
};