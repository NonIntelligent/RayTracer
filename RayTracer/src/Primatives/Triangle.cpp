#include "Primatives/Triangle.h"

triangle::triangle(vec3 pos, vec3 v0, vec3 v1, vec3 v2, vec3 color) {

}

triangle::~triangle() {

}

bool triangle::rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) {
	return true;
}

void triangle::computeColour(Light* light, const vec3 viewDir, const vec3 intersectPt, vec3& result) {
	
}
