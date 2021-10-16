#pragma once
#include "glm/glm.hpp"
#include "Light.h"

using namespace glm;

class Model {
protected:
	vec3 center;

public:
	vec3 position;
	vec3 mycolour;
	float shininess = 0;

	virtual bool rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) = 0;
	virtual void computeColour(Light* light, const vec3 viewDir, const vec3 intersectPt, vec3& result) = 0;

	void getSurfaceData(vec3 rayOrigin, vec3 rayDirection, float t, vec3& intersectPt, vec3& normVec);
};

