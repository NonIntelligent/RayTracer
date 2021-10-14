#pragma once
#include "glm/vec3.hpp"
using namespace glm;

class Model {
private:
	vec3 center;
	vec3 diffuseColour;
	vec3 diffuseIntensity;

public:
	vec3 position;
	vec3 mcolour;

	virtual bool rayIntersect(vec3 rayDirection, vec3 rayOrigin, float& t, vec3& IntPt, vec3& normVec) = 0;
	virtual void computeColour() = 0;
};

