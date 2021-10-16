#pragma once
#include "Primatives/Model.h"

using namespace glm;

class Sphere : Model {
private:
	float radius;
public:
	Sphere() {};
	Sphere(float, vec3, vec3);
	~Sphere();
	float getRadius(void);
	vec3 getCenter(void);

	bool rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) override;

	void computeColour(Light* light, const vec3 viewDir, const vec3 intersectPt, vec3& result) override;

};