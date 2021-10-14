#pragma once

#include "Primatives/Model.h"

class triangle : public Model {
private:
	vec3 vertex0, vertex1, vertex2;
	vec3 norm0, norm1, norm2;

public:
	triangle(vec3 pos, vec3 v0, vec3 v1, vec3 v2, vec3 color);
	triangle(vec3 pos, vec3 v0, vec3 v1, vec3 v2,
		vec3 n0, vec3 n1, vec3 n2,
		vec3 colour) :
		vertex0(v0), vertex1(v1), vertex2(v2), norm0(n0), norm1(n1), norm2(n2)
	{
		mcolour = colour;
	};

	bool rayIntersect(vec3 rayDirection, vec3 rayOrigin, float& t, vec3& IntPt, vec3& normVec)override;
	void ComputeColor(const float ambientIntensity, const vec3 IntPt, const vec3 lightPt, const vec3 rayDirection, const vec3 tNormvec, float& ColValue);
	~triangle();
};