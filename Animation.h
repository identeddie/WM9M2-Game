#pragma once
#include <vector>
#include <string>
#include <map>
#include "mathlib.h"

struct Bone {
	std::string name;
	Matrix offset;
	int parentIndex;
};

struct Skeleton {
	std::vector<Bone> bones;
	Matrix globalInverse;
};

struct AnimationFrame {
	std::vector<Vec3> positions;
	std::vector<Quaternion> rotations;
	std::vector<Vec3> scales;
};

class AnimationSequence {
public:
	std::vector<AnimationFrame> frames;
	float ticksPerSecond;

	Vec3 interpolate(Vec3& p1, Vec3& p2, float t) {
		return lerp<Vec3>(p1, p2, t);
	}

	Quaternion interpolate(Quaternion& q1, Quaternion& q2, float t) {
		return slerp(q1, q2, t);
	}

	float duration() {
		return ((float)frames.size() / ticksPerSecond);
	}

	void calcFrame(float t, int& frame, float& interpolationFact) {
		interpolationFact = t * ticksPerSecond;
		frame = (int)floorf(interpolationFact);
		interpolationFact = interpolationFact - (float)frame;
		frame = mn(frame, frames.size() - 1);
	}

	int nextFrame(int frame) {
		return mn(frame + 1, frames.size() - 1);
	}

	Matrix interpolateBoneToGlobal(Matrix* matrices, int baseFrame, float interpolationFact, Skeleton* skeleton, int boneIndex) {
		int nextFrameIndex = nextFrame(baseFrame);
		Matrix scale = Matrix::scaling(interpolate(frames[baseFrame].scales[boneIndex], frames[nextFrameIndex].scales[boneIndex], interpolationFact));
		Matrix rotation = interpolate(frames[baseFrame].rotations[boneIndex], frames[nextFrameIndex].rotations[boneIndex], interpolationFact).toMatrix();
		Matrix translation = Matrix::translation(interpolate(frames[baseFrame].positions[boneIndex], frames[nextFrameIndex].positions[boneIndex], interpolationFact));
		//Matrix scale = Matrix::scaling(frames[baseFrame].scales[boneIndex]);
		//Matrix rotation = frames[baseFrame].rotations[boneIndex].toMatrix();
		//Matrix translation = Matrix::translation(frames[baseFrame].positions[boneIndex]);
		Matrix local = scale * rotation * translation;

		if (skeleton->bones[boneIndex].parentIndex > -1) {
			Matrix global = local * matrices[skeleton->bones[boneIndex].parentIndex];
			return global;
		}

		return local;
	}
};

class Animation {
public:
	std::map<std::string, AnimationSequence> animations;
	Skeleton skeleton;

	void calcFrame(std::string name, float t, int& frame, float& interpolationFact) {
		animations[name].calcFrame(t, frame, interpolationFact);
	}

	Matrix interpolateBoneToGlobal(std::string name, Matrix* matrices, int baseFrame, float interpolationFact, int boneIndex) {
		return animations[name].interpolateBoneToGlobal(matrices, baseFrame, interpolationFact, &skeleton, boneIndex);
	}

	void calcFinalTransforms(Matrix* matrices) {
		for (int i = 0; i < skeleton.bones.size(); i++) {
			matrices[i] = skeleton.bones[i].offset * matrices[i] * skeleton.globalInverse;
		}
	}
};

class AnimationInstance {
public:
	Animation* animation;
	std::string currentAnimation;
	float t;
	Matrix matrices[256];

	void init(Animation* _animation) {
		animation = _animation;
		t = 0;
	}

	void resetAnimationTime() {
		t = 0;
	}

	bool animationFinished() {
		if (t > animation->animations[currentAnimation].duration()) {
			return true;
		}
		return false;
	}

	void update(std::string name, float dt) {
		if (name == currentAnimation) {
			t += dt;
		} else {
			currentAnimation = name;
			t = 0;
		}

		if (animationFinished() == true) { 
			resetAnimationTime();
		}

		int frame = 0;
		float interpolationFact = 0;
		animation->calcFrame(name, t, frame, interpolationFact);

		for (int i = 0; i < animation->skeleton.bones.size(); i++) {
			matrices[i] = animation->interpolateBoneToGlobal(name, matrices, frame, interpolationFact, i);
		}

		animation->calcFinalTransforms(matrices/*, coordTransform*/);
	}
};

