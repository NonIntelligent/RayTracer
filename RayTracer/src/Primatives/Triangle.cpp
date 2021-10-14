#include "Primatives/Triangle.h"

triangle::triangle(vec3 pos, vec3 v0, vec3 v1, vec3 v2, vec3 color) {

}

bool triangle::rayIntersect(vec3 rayDirection, vec3 rayOrigin, float& t, vec3& IntPt, vec3& normVec) {
	return true;
}

void triangle::ComputeColor(const float ambientIntensity, const vec3 IntPt, const vec3 lightPt, const vec3 rayDirection, const vec3 tNormvec, float& ColValue) {

}

triangle::~triangle() {

}
