#pragma once
#include "mathlib.h"
#include "Window.h"

class Persective {
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

		if (fwd == up) { //If colinear, change to (0, 1, 0)
			up.x = 1.f;
			up.y = 0.f;
		}

		right = up.cross(fwd);
		right.normalize();

		up = fwd.cross(right);
		up.normalize();
	}

	Persective() {
		pos = Vec3(0.f);
		dir = SphericalCoord(0.f, 0.f);
		up = Vec3(0.f, 1.f, 0.f);

		speed = 1.f;
		sensitivity = 1.f;
	}

	Persective(Vec3& _pos) {
		pos = _pos;
		dir = SphericalCoord(0.f, 0.f);
		up = Vec3(0.f, 1.f, 0.f);

		speed = 5.f;
		sensitivity = 0.1f;
	}

	void gander(float dx, float dy) {
		dx *= sensitivity;
		dy *= sensitivity;

		if (dx != 0.f && dy != 0.f) {
			dir.theta += dy;
			dir.phi += dx;

			if (dir.theta >= M_PI) {
				dir.theta -= M_PI;
			} else if (dir.theta < 0.f) {
				dir.theta += M_PI;
			}

			if (dir.phi >= M_PI * 2) {
				dir.phi -= M_PI * 2;
			} else if (dir.phi < 0.f) {
				dir.phi += M_PI * 2;
			}

			GramSchmit();
		}
	}

	void meander(Window& win, float dt) {
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
			float k = (speed * dt) / mag;
			posChange = posChange * k; //Normalize movement vector, cap movement at sprite's speed value

			pos += posChange;
		}
	}
};