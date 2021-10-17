#pragma once
#include "Primatives/Model.h"

using namespace glm;

class Sphere : Model {
private:
	float radius = 4.f;
public:
	Sphere() {};
	Sphere(float, vec3, vec3);
	~Sphere();
	float getRadius(void);

	bool rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) override;

};