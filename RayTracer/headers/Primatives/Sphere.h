#pragma once
#include "Primatives/Model.h"
#include <glm/vec3.hpp>

using namespace glm;

class Sphere : Model {
private:
	float radius;
	vec3  center;
	vec3 mycolor;
public:
	Sphere() {
	};
	Sphere(float, vec3, vec3);
	~Sphere();
	float getRadius(void);
	vec3 getCenter(void);
	vec3 getMyColor(void);

	bool rayIntersect(vec3 rayDirection, vec3 rayOrigin, float& t, vec3& IntPt, vec3& normVec) override;


	void computeColour() override;

};