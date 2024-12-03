	#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <memory.h>
#include <iostream>

#define sq(x) (x) * (x)
#define mx(a, b) (a > b ? a : b)
#define mn(a, b) (a < b ? a : b)

template<typename T>
static T lerp(const T a, const T b, float t)
{
    return a * (1.0f - t) + (b * t);
}

class Vec2 {
public:
	float x, y;

	Vec2() {
		x = y = 0;
	}

	Vec2(float _x) {
		x = y = _x;
	}

	Vec2(float _x, float _y) {
		x = _x;
		y = _y;
	}

	Vec2 operator+(const Vec2 v) { //Elementwise addition
		Vec2 newVec;

		newVec.x = x + v.x;
		newVec.y = y + v.y;

		return newVec;
	}

	Vec2 operator-(const Vec2 v) { //Elementwise subtraction
		Vec2 newVec;

		newVec.x = x - v.x;
		newVec.y = y - v.y;

		return newVec;
	}

	Vec2 operator*(float k) { //Multiply each element by constant k
		Vec2 newVec;

		newVec.x = x * k;
		newVec.y = y * k;

		return newVec;
	}

	Vec2 operator*(const Vec2 v) { //Elementwise multiplication
		Vec2 newVec;

		newVec.x = x * v.x;
		newVec.y = y * v.y;

		return newVec;
	}

	Vec2 operator/(float k) { //Elementwise division by constant k
		Vec2 newVec;

		newVec.x = x / k;
		newVec.y = y / k;

		return newVec;
	}

	void setPos(float _x, float _y) {
		x = _x;
		y = _y;
	}

	float length() {
		return sqrtf(x * x + y * y);
	}

	Vec2 normalize() { //Normalize vector to unit length
		Vec2 newVec;

		newVec.x = x / length();
		newVec.y = y / length();

		return newVec;
	}

	void display() {
		std::cout << "X: " << x << " Y: " << y << std::endl;
	}
};

class Vec3 {
public:
	union {
		float v[3];
		struct { float x, y, z;  };
		struct { Vec2 pos2D; float z; };
	};

	Vec3() {
		x = y = z = 0;
	}

	Vec3(float _x) {
		x = y = z = _x;
	}

	Vec3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vec3 operator+(const Vec3& vec) const { //Elementwise addition
		return Vec3(v[0] + vec.v[0], v[1] + vec.v[1], v[2] + vec.v[2]);
	}

	Vec3& operator+=(const Vec3& vec) {
		v[0] += vec.v[0];
		v[1] += vec.v[1];
		v[2] += vec.v[2];
		return *this;
	}

	Vec3& operator+=(const float val) {
		v[0] += val;
		v[1] += val;
		v[2] += val;
		return *this;
	}

	Vec3 operator-(const Vec3& vec) {
		return Vec3(v[0] - vec.v[0], v[1] - vec.v[1], v[2] - vec.v[2]);
	}

	Vec3& operator-=(const Vec3& vec) {
		v[0] -= vec.v[0];
		v[1] -= vec.v[1];
		v[2] -= vec.v[2];
		return *this;
	}

	Vec3& operator-=(const float val) {
		v[0] -= val;
		v[1] -= val;
		v[2] -= val;
		return *this;
	}

	Vec3 operator*(const Vec3& vec) const {
		return Vec3(v[0] * vec.v[0], v[1] * vec.v[1], v[2] * vec.v[2]);
	}

	Vec3 operator*(const float val) const {
		return Vec3(v[0] * val, v[1] * val, v[2] * val);
	}

	Vec3& operator*=(const Vec3& vec) {
		v[0] *= vec.v[0];
		v[1] *= vec.v[1];
		v[2] *= vec.v[2];
		return *this;
	}

	Vec3& operator*=(const float val) {
		v[0] *= val;
		v[1] *= val;
		v[2] *= val;
		return *this;
	}

	Vec3 operator/(const float val) const {
		return Vec3(v[0] / val, v[1] / val, v[2] / val);
	}

	Vec3& operator/=(const float val) {
		v[0] /= val;
		v[1] /= val;
		v[2] /= val;
		return *this;
	}

	Vec3 operator-() const {
		return Vec3(-x, -y, -z);
	}

	bool operator==(const Vec3& vec) {
		if (x == vec.x && y == vec.y && z == vec.z) {
			return true;
		} else {
			return false;
		}
	}

	float mag() const {
		return sqrtf(sq(x) + sq(y) + sq(z));
	}

	float magSq() const {
		return sq(x) + sq(y) + sq(z);
	}

	Vec3& normalize(void) {
		float length = 1.0f / sqrtf(sq(x) + sq(y) + sq(z));
		x *= length;
		y *= length;
		z *= length;
		return *this;
	}

	float normalizeGetLength() {
		float length = sqrtf(sq(x) + sq(y) + sq(z));
		float l = 1.0f / length;

		x *= l;
		y *= l;
		z *= l;
		return length;
	}

	float dot(const Vec3& vec) const {
		return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2];
	}

	Vec3 cross(const Vec3& vec) {
		return Vec3(vec.v[1] * v[2] - vec.v[2] * v[1],
					vec.v[2] * v[0] - vec.v[0] * v[2],
				    vec.v[0] * v[1] - vec.v[1] * v[0]);
	}

	float Max() const {
		return mx(x, mx(y, z));
	}

	float Min() const {
		return mn(x, mn(y, z));
	}

	void display() {
		std::cout << "Vec3:" << std::endl;
		std::cout << x << std::endl << y << std::endl << z << std::endl;
 	}

};

float dot(const Vec3& v1, const Vec3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3 Max(const Vec3& v1, const Vec3& v2) {
	return Vec3(mx(v1.x, v2.x), mx(v1.y, v2.y), mx(v1.z, v2.z));
}	

Vec3 Min(const Vec3& v1, const Vec3& v2) {
	return Vec3(mn(v1.x, v2.x), mn(v1.y, v2.y), mn(v1.z, v2.z));
}

class SphericalCoord {
public:
	union {
		float v[3];
		struct { float r, theta, phi; };
	};

	SphericalCoord() {
		r = 1.f;
		theta = phi = 0.f;
	}

	SphericalCoord(float _theta, float _phi) {
		theta = _theta;
		phi = _phi;
		r = 1.f;
	}

	/*SphericalCoord(float x, float y, float z) {
		r = 1.f;

		theta = arcos
	}*/

	Vec3 toCartesianCam() {
		float sinTheta = sinf(theta);
		return Vec3(r * sinTheta * cosf(phi), r * cosf(theta), r * sinTheta * sinf(phi));
	}

	Vec3 toCartesianShade() {
		float sinTheta = sinf(theta);
		return Vec3(r * sinTheta * cosf(phi), r * sinTheta * sinf(phi), r * cosf(theta));
	}

};

class Vec4 {
public:
	union {
		float v[4];
		struct { float x, y, z, w; };
		struct { Vec3 vec3; float w; };
	};

	Vec4() {
		x = y = z = 0.f;
		w = 1.f;
	}

	Vec4(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
		w = 1.f;
	}

	Vec4(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	Vec4(Vec3& _vec3) {
		vec3 = _vec3;
		w = 1.f;
	}

	void perspectiveDiv() {
		vec3 /= w;
		w = 1.f / w;
	}

	Vec2 toScreen(int width, int height) {
		x = (x + 1) * width / 2;
		y = ((y + 1) * height / 2);

		return vec3.pos2D;
	}

};

class Matrix {
public:
	union {
		float a[4][4];
		float m[16];
	};

	void eye() {
		memset(m, 0, 16 * sizeof(float));
		m[0] = 1.0f;
		m[5] = 1.0f;
		m[10] = 1.0f;
		m[15] = 1.0f;
	}

	Matrix() {
		eye();
	}

	Matrix(float* otherMatrix) {
		memcpy(&m, otherMatrix, 16 * sizeof(float));
	}

	Matrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) {
		a[0][0] = m00;
		a[0][1] = m01;
		a[0][2] = m02;
		a[0][3] = m03;
		a[1][0] = m10;
		a[1][1] = m11;
		a[1][2] = m12;
		a[1][3] = m13;
		a[2][0] = m20;
		a[2][1] = m21;
		a[2][2] = m22;
		a[2][3] = m23;
		a[3][0] = m30;
		a[3][1] = m31;
		a[3][2] = m32;
		a[3][3] = m33;
	}

	float& operator[](int idx) {
		return m[idx];
	}

	Vec3 mulPoint(const Vec3& vec) {
		Vec3 v1 = Vec3(
			(vec.x * m[0] + vec.y * m[1] + vec.z * m[2]) + m[3],
			(vec.x * m[4] + vec.y * m[5] + vec.z * m[6]) + m[7],
			(vec.x * m[8] + vec.y * m[9] + vec.z * m[10]) + m[11]);

		float w;
		w = (m[12] * vec.x) + (m[13] * vec.y) + (m[14] * vec.z) + m[15];
		w = 1.0f / w;
		return (v1 * w);
	}

	Vec3 mulVec(const Vec3& vec) {
		return Vec3(
			(vec.x * m[0] + vec.y * m[1] + vec.z * m[2]),
			(vec.x * m[4] + vec.y * m[5] + vec.z * m[6]),
			(vec.x * m[8] + vec.y * m[9] + vec.z * m[10]));
	}

	Vec4 mulVec(const Vec4& vec) {
		return Vec4(
			(vec.x * m[0] + vec.y * m[1] + vec.z * m[2] + vec.w * m[3]),
			(vec.x * m[4] + vec.y * m[5] + vec.z * m[6] + vec.w * m[7]),
			(vec.x * m[8] + vec.y * m[9] + vec.z * m[10] + vec.w * m[11]),
			(vec.x * m[12] + vec.y * m[13] + vec.z * m[14] + vec.w * m[15]));
	}

	Matrix mul(const Matrix& matrix) const {
		Matrix ret;
		ret.m[0] = m[0] * matrix.m[0] + m[4] * matrix.m[1] + m[8] * matrix.m[2] + m[12] * matrix.m[3];
		ret.m[1] = m[1] * matrix.m[0] + m[5] * matrix.m[1] + m[9] * matrix.m[2] + m[13] * matrix.m[3];
		ret.m[2] = m[2] * matrix.m[0] + m[6] * matrix.m[1] + m[10] * matrix.m[2] + m[14] * matrix.m[3];
		ret.m[3] = m[3] * matrix.m[0] + m[7] * matrix.m[1] + m[11] * matrix.m[2] + m[15] * matrix.m[3];
		ret.m[4] = m[0] * matrix.m[4] + m[4] * matrix.m[5] + m[8] * matrix.m[6] + m[12] * matrix.m[7];
		ret.m[5] = m[1] * matrix.m[4] + m[5] * matrix.m[5] + m[9] * matrix.m[6] + m[13] * matrix.m[7];
		ret.m[6] = m[2] * matrix.m[4] + m[6] * matrix.m[5] + m[10] * matrix.m[6] + m[14] * matrix.m[7];
		ret.m[7] = m[3] * matrix.m[4] + m[7] * matrix.m[5] + m[11] * matrix.m[6] + m[15] * matrix.m[7];
		ret.m[8] = m[0] * matrix.m[8] + m[4] * matrix.m[9] + m[8] * matrix.m[10] + m[12] * matrix.m[11];
		ret.m[9] = m[1] * matrix.m[8] + m[5] * matrix.m[9] + m[9] * matrix.m[10] + m[13] * matrix.m[11];
		ret.m[10] = m[2] * matrix.m[8] + m[6] * matrix.m[9] + m[10] * matrix.m[10] + m[14] * matrix.m[11];
		ret.m[11] = m[3] * matrix.m[8] + m[7] * matrix.m[9] + m[11] * matrix.m[10] + m[15] * matrix.m[11];
		ret.m[12] = m[0] * matrix.m[12] + m[4] * matrix.m[13] + m[8] * matrix.m[14] + m[12] * matrix.m[15];
		ret.m[13] = m[1] * matrix.m[12] + m[5] * matrix.m[13] + m[9] * matrix.m[14] + m[13] * matrix.m[15];
		ret.m[14] = m[2] * matrix.m[12] + m[6] * matrix.m[13] + m[10] * matrix.m[14] + m[14] * matrix.m[15];
		ret.m[15] = m[3] * matrix.m[12] + m[7] * matrix.m[13] + m[11] * matrix.m[14] + m[15] * matrix.m[15];
		return ret;
	}

	float det() {
		float inv0 = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
		float inv4 = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
		float inv8 = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
		float inv12 = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

		return m[0] * inv0 + m[1] * inv4 + m[2] * inv8 + m[3] * inv12;
	}

	Matrix invert() {
		Matrix inv;
		inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
		inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
		inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
		inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
		inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
		inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
		inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
		inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
		inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
		inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
		inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
		inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
		inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
		inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
		inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
		inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

		float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

		if (det == 0) {
			// Handle this case
		}

		det = 1.f / det;
		for (int i = 0; i < 16; i++) {
			inv[i] = inv[i] * det;
		}
		return inv;
	}

	Matrix T() {
		Matrix T(m);

		T[1] = m[4];
		T[4] = m[1];
		T[2] = m[8];
		T[8] = m[2];
		T[3] = m[12];
		T[12] = m[3];
		T[6] = m[9];
		T[9] = m[6];
		T[7] = m[13];
		T[13] = m[7];
		T[11] = m[14];
		T[14] = m[11];
		T[0] = m[0];
		T[5] = m[5];
		T[10] = m[10];
		T[15] = m[15];

		return T;
	}

	Matrix operator*(const Matrix& matrix) {
		return mul(matrix);
	}

	static Matrix translation(const Vec3& vec) {
		Matrix mat;
		mat.a[0][3] = vec.x;
		mat.a[1][3] = vec.y;
		mat.a[2][3] = vec.z;

		return mat;
	};

	static Matrix scaling(const Vec3& vec) {
		Matrix mat;
		mat.m[0] = vec.x;
		mat.m[5] = vec.y;
		mat.m[10] = vec.z;

		return mat;
	}

	static Matrix rotateX(float theta) {
		Matrix mat;

		float cos = cosf(theta);
		float sin = sinf(theta);

		mat.m[5] = cos;
		mat.m[6] = -sin;
		mat.m[9] = sin;
		mat.m[10] = cos;

		return mat;
	}

	static Matrix rotateY(float theta) {
		Matrix mat;

		float cos = cosf(theta);
		float sin = sinf(theta);

		mat.m[0] = cos;
		mat.m[2] = sin;
		mat.m[8] = -sin;
		mat.m[10] = cos;

		return mat;
	}

	static Matrix rotateZ(float theta) {
		Matrix mat;

		float cos = cosf(theta);
		float sin = sinf(theta);

		mat.m[0] = cos;
		mat.m[1] = -sin;
		mat.m[4] = sin;
		mat.m[5] = cos;

		return mat;
	}

	static Matrix perspectiveProj(float aspect, float fovTheta, float nr, float fr) { //nr (near), fr (far)
		Matrix mat;

		mat[0] = 1 / (aspect * tanf(fovTheta / 2));
		mat[5] = 1 / tanf(fovTheta / 2);
		mat[10] = -fr / (fr - nr);
		mat[11] = -fr * nr / (fr - nr);
		mat[14] = -1.f;
		mat[15] = 0;

		return mat;
	}

	static Matrix lookAt(Vec3& from, Vec3& to) {
		Vec3 dir = from - to;
		dir.normalize();

		Vec3 up(0.f, 1.f, 0.f); //Guess for up vector

		if (dir == up) { //If colinear, change to (0, 1, 0)
			up.x = 1.f;
			up.y = 0.f;
		}

		Vec3 right = up.cross(dir);
		right.normalize();

		up = dir.cross(right);
		up.normalize();

		Matrix lookat(right.x, right.y, right.z, -from.dot(right),
				   up.x, up.y, up.z, -from.dot(up),
				   dir.x, dir.y, dir.z, -from.dot(dir),
				   0, 0, 0, 1);

		return lookat;
	}

	void display() {
		std::cout << "Matrix:" << std::endl;
		std::cout << m[0] << "\t" << m[1] << "\t" << m[2] << "\t" << m[3] << std::endl;
		std::cout << m[4] << "\t" << m[5] << "\t" << m[6] << "\t" << m[7] << std::endl;
		std::cout << m[8] << "\t" << m[9] << "\t" << m[10] << "\t" << m[11] << std::endl;
		std::cout << m[12] << "\t" << m[13] << "\t" << m[14] << "\t" << m[15] << std::endl;

	}

};

class ShadingFrame {
public:
	union {
		float a[3][3];
		float m[9];
		struct { Vec3 t, b, n; };
	};

	ShadingFrame() {
		memset(m, 0, 9 * sizeof(float));
		m[0] = 1.f;
		m[4] = 1.f;
		m[8] = 1.f;
	}

	ShadingFrame(Vec3& _t, Vec3& _b, Vec3& _n) {
		t = _t;
		b = _b;
		n = _n;
	}

	ShadingFrame(Vec3& vec) {
		vec.normalize();
		n = vec;

		if (t.x == 1.f && t.y == 0.f && t.z == 0.f) {
			b.y = 1.f;
		} else {
			b.x = 1.f;
		}

		t = n.cross(b);
		t.normalize();
		b = n.cross(t);
	}

	ShadingFrame(float* otherSF) {
		memcpy(&m, otherSF, 9 * sizeof(float));
	}

	float& operator[](int idx) {
		return m[idx];
	}

	ShadingFrame T() {
		ShadingFrame T(m);

		T[1] = m[3];
		T[3] = m[1];
		T[2] = m[6];
		T[6] = m[2];
		T[5] = m[7];
		T[7] = m[5];

		return T;
	}

	ShadingFrame inv() {
		ShadingFrame inv(m);

		return inv.T();
	}

	void display() {
		std::cout << "Shading Frame:" << std::endl;
		std::cout << m[0] << "\t" << m[1] << "\t" << m[2] << std::endl;
		std::cout << m[3] << "\t" << m[4] << "\t" << m[5] << std::endl;
		std::cout << m[6] << "\t" << m[7] << "\t" << m[8] << std::endl;
	}
};

class Quaternion {
public:
	union {
		struct { float a, b, c, d; };
		float q[4];
	};

	Quaternion() {
		memset(q, 0, 4 * sizeof(float));
	}

	Quaternion(float _a, float _b, float _c, float _d) {
		a = _a;
		b = _b;
		c = _c;
		d = _d;
	}

	Quaternion(Vec3& v, float theta) {
		float sn = sinf(theta / 2.f);

		a = cosf(theta / 2.f);
		b = v.x * sn;
		c = v.y * sn;
		d = v.z * sn;
	}

	Quaternion operator*(const float k) {
		return Quaternion(a * k, b * k, c * k, d * k);
	}

	Quaternion operator+(const Quaternion& quat) {
		return Quaternion(a + quat.a, b + quat.b, c + quat.c, d + quat.d);
	}

	Quaternion operator*(const Quaternion& quat) {
		return Quaternion(a * quat.a - b * quat.b - c * quat.c - d * quat.d,
			a * quat.b + b * quat.a + c * quat.d - d * quat.c,
			a * quat.c - b * quat.d + c * quat.a + d * quat.b,
			a * quat.d + b * quat.c - c * quat.b + d * quat.a);
	}

	Quaternion operator-() const {
		return Quaternion(-a, -b, -c, -d);
	}

	float mag() {
		return sqrtf(sq(a) + sq(b) + sq(c) + sq(d));
	}

	Quaternion& normalize() {
		float length = 1.0f / sqrtf(sq(a) + sq(b) + sq(c) + sq(d));
		a *= length;
		b *= length;
		c *= length;
		d *= length;

		return *this;
	}

	Quaternion& conj() {
		b = -b;
		c = -c;
		d = -d;

		return *this;
	}

	Quaternion& inv() {
		float length = 1.0f / sqrtf(sq(a) + sq(b) + sq(c) + sq(d));

		this->conj();

		a *= length;
		b *= length;
		c *= length;
		d *= length;

		return *this;
	}

	float dot(Quaternion& q2) {
		return (a * q2.a) + (b * q2.b) + (c * q2.c) + (d * q2.d);
	}

	Quaternion slerp(Quaternion& q2, float t) {
		float theta = acosf(this->dot(q2));

		float oneOverSinTheta = 1 / sinf(theta);

		Quaternion ret = (*this * (sinf(theta * (1 - t)) * oneOverSinTheta)) + (q2 * (sinf(theta * t) * oneOverSinTheta));

		return ret;
	}

	Matrix toMatrix() {
		float xx = q[0] * q[0];
	float xy = q[0] * q[1];
	float xz = q[0] * q[2];
	float yy = q[1] * q[1];
	float zz = q[2] * q[2];
	float yz = q[1] * q[2];
	float wx = q[3] * q[0];
	float wy = q[3] * q[1];
	float wz = q[3] * q[2];
	Matrix matrix;
	matrix[0] = 1.0f - 2.0f * (yy + zz);
	matrix[1] = 2.0f * (xy - wz);
	matrix[2] = 2.0f * (xz + wy);
	matrix[3] = 0.0;
	matrix[4] = 2.0f * (xy + wz);
	matrix[5] = 1.0f - 2.0f * (xx + zz);
	matrix[6] = 2.0f * (yz - wx);
	matrix[7] = 0.0;
	matrix[8] = 2.0f * (xz - wy);
	matrix[9] = 2.0f * (yz + wx);
	matrix[10] = 1.0f - 2.0f * (xx + yy);
	matrix[11] = 0.0;
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
	return matrix;
	}	
};

float dot(Quaternion& q1, Quaternion& q2) {
	return (q1.a * q2.a) + (q1.b * q2.b) + (q1.c * q2.c) + (q1.d * q2.d);
}

Quaternion slerp(Quaternion& q1, Quaternion& q2, float t) {
	float dotProd = q1.dot(q2);

	if (dotProd < 0.f) {
		dotProd = -q1.dot(q2);
	}

 	float theta = acosf(dotProd);

	if (theta < 0.0001f) {
		return q1 * (1 - t) + q2 * t;
	}

	float oneOverSinTheta = 1.f / sinf(theta);

	Quaternion ret = (q1 * (sinf(theta * (1.f - t)) * oneOverSinTheta)) + (q2 * (sinf(theta * t) * oneOverSinTheta));
	return ret;
}

class Color {
public:
	float r, g, b, a;

	Color() {
		r = g = b = 0.f;
		a = 1.f; //Default to fully opaque
	}

	Color(float _r, float _g, float _b) {
		r = _r;
		g = _g;
		b = _b;
		a = 1.f; //Assume fully opaque
	}

	Color(float _r, float _g, float _b, float _a) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	Color(uint8_t _r, uint8_t _g, uint8_t _b) {
		r = _r / 255.f;
		g = _g / 255.f;
		b = _b / 255.f;
		a = 1.f;
	}

	Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) {
		r = _r / 255.f;
		g = _g / 255.f;
		b = _b / 255.f;
		a = _a / 255.f;
	}

	Color operator+(const Color& colorIn) {
		Color colorOut;
		
		colorOut.r = r + colorIn.r;
		colorOut.g = g + colorIn.g;
		colorOut.b = b + colorIn.b;

		return colorOut;
	}

	Color operator*(const Color& colorIn) {
		Color colorOut;

		colorOut.r = r * colorIn.r;
		colorOut.g = g * colorIn.g;
		colorOut.b = b * colorIn.b;

		return colorOut;
	}

	Color operator*(const float a) {
		Color colorOut;

		return colorOut;
	}

	void display() {
		std::cout << "R: " << r << " G: " << g << " B: " << b << " A: " << a << std::endl;
	}
};