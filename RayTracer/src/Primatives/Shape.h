#pragma once
#include "glm/vec3.hpp"
using namespace glm;

class Shape {
private:
	vec3 center;
	vec3 diffuseColour;
	vec3 diffuseIntensity;

public:
	vec3 position;
	vec3 mcolour;

	Shape();
	~Shape();
	virtual bool rayIntersect(vec3 rayDirection, vec3 rayOrigin, float& t, vec3& IntPt, vec3& normVec);
	virtual void computeColour();
};

