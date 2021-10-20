#pragma once
#include "Model.h"

class Plane : public Model {
	vec3 planeNormal = vec3(1.0, 1.0, 0.0);

public:
	Plane() { };
	Plane(vec3 position, vec3 colour, vec3 normal);
	~Plane() { };

	bool rayIntersect(vec3 rayOrigin, vec3 rayDirection, IntersectData& data) override;

};

