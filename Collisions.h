#pragma once

#include "mathlib.h"


class AABB {
public:
    Vec3 max;
    Vec3 min;

    AABB() {
        reset();
    }

    AABB& operator+=(const Vec3 dPos) {
        max += dPos;
        min += dPos;
        return *this;
    }

    AABB operator+(const Vec3 dPos) {
        AABB ret;
        ret.max = max + dPos;
        ret.min = min + dPos;
        return ret;
    }

    void reset() {
        max = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        min = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    }

    void extend(const Vec3& p) {
        max = Max(max, p);
        min = Min(min, p);
    }

    Vec3 checkCollision(AABB& otherBB) {
        float PDx = mn(max.x, otherBB.max.x) - max(min.x, otherBB.min.x);
        float PDy = mn(max.y, otherBB.max.y) - max(min.y, otherBB.min.y);
        float PDz = mn(max.z, otherBB.max.z) - max(min.z, otherBB.min.z);

        //return Vec3(PDx, PDy, PDz);

        if (PDx > 0 && PDy > 0 && PDz > 0) {
            if (min.x < otherBB.min.x) {
                PDx *= -1;
            }

            if (min.y < otherBB.min.y) {
                PDy *= -1;
            }

            if (min.z < otherBB.min.z) {
                PDz *= -1;
            }

            return Vec3(PDx, PDy, PDz);
        } else {
            return Vec3(0.f);
        }
    }

    void reduceXY(float kx, float kz) {
        float xDiff = max.x - min.x;
        float zDiff = max.z - min.z;
        float xScale = kx * xDiff / 2.f;
        float zScale = kz * zDiff / 2.f ;

        max.x -= xScale;
        min.x += xScale;
        max.z -= zScale;
        min.z += zScale;
    }
};
