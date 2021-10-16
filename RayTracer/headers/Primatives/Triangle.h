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
		mycolour = colour;
	};
	~triangle();

	bool rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t)override;

	void computeColour(Light* light, const vec3 viewDir, const vec3 intersectPt, vec3& result) override;

};