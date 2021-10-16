#include "Primatives/Model.h"

void Model::getSurfaceData(vec3 rayOrigin, vec3 rayDirection, float t, vec3& intersectPt, vec3& normVec) {
	// Calculate intersection point and normal
	intersectPt = rayOrigin + rayDirection * t;
	normVec = normalize(intersectPt - center);
}
