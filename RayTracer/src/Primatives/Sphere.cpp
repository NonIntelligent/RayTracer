#include "Primatives/Sphere.h"

Sphere::Sphere(float r, vec3 cen, vec3 col) {
	radius = r;
	center = cen;
	mycolor = col;
}

Sphere::~Sphere() {
}

float Sphere::getRadius(void) {
	return radius;
}

vec3 Sphere::getCenter(void) {
	return center;
}

vec3 Sphere::getMyColor(void) {
	return mycolor;
}

bool Sphere::rayIntersect(vec3 rayDirection, vec3 rayOrigin, float& t, vec3& IntPt, vec3& normVec) {
	return true;
}

void Sphere::computeColour() {

}
