#pragma once
#include "glm/glm.hpp"
#include "Light.h"

using namespace glm;

class Model {
protected:
	vec3 centre;

public:
	vec3 mycolour;
	float shininess = 0;

	virtual bool rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) = 0;
	virtual void computeColour(Light* light, const vec3 viewDir, const vec3 intersectPt, vec3& result) const;

	virtual void getSurfaceData(vec3 rayOrigin, vec3 rayDirection, float t, vec3& intersectPt, vec3& normVec) const;
	vec3 getCentre();
};

