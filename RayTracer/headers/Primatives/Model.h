#pragma once
#include "glm/glm.hpp"
#include "Light.h"
#include <vector>

#define epsilon 0.0000001f
#define SHADOW_NONE 1.f
#define SHADOW_HARD 0.f
const float SHADOW_BIAS = 0.01f;

class Model;

using namespace glm;

struct IntersectData {
	vec3 rayorigin;
	vec3 intersectPoint;
	vec3 normal;
	float t;

	// only to be used by triangles
	vec3 UVs;

	float shadowGradient;
	float occluderDistance;
	float density; // 1 = not reflective and 0 is a window
	Model* model = nullptr;
};

class Model {
protected:
	vec3 centre;

public:
	vec3 mycolour;
	float shininess = 0.f;
	float density = 1.f;

	virtual bool rayIntersect(vec3 rayOrigin, vec3 rayDirection, IntersectData& data) = 0;
	virtual void computeColour(Light* light, const vec3 viewDir, const IntersectData data, vec3& result) const;

	inline void setCentre(vec3 pos) {centre = pos;};
	vec3 getCentre();
};

