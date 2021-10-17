#pragma once

#include "Primatives/Model.h"

class Triangle : public Model {
private:
	vec3 vertex0, vertex1, vertex2;
	vec3 norm0, norm1, norm2;

	vec3 vertexColours[3];

public:
	float u, v, w;

	Triangle();
	// Simple Triangle, normals need to be calculated
	Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 colour);

	// Normals supplied by mesh obj
	Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 colour, float shininess);

	~Triangle();

	bool rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) override;

	void computeColour(Light* light, const vec3 viewDir, const vec3 intersectPt, vec3& result) const override;

	void getSurfaceData(vec3 rayOrigin, vec3 rayDirection, float t, vec3& intersectPt, vec3& normVec) const override;

	vec3 findAndSetCentre();
};