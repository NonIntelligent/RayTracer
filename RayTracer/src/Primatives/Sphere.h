#pragma once
#include "Primatives/Shape.h"
#include <glm/vec3.hpp>

using namespace glm;

class Sphere : Shape {
private:
	float radius;
	vec3  center;
	vec3 mycolor;
public:
	Sphere(float, vec3, vec3);
	~Sphere();
	float getRadius(void);
	vec3 getCenter(void);
	vec3 getMyColor(void);
};