#include "Sphere.h"

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
