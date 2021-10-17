#include "Primatives/Sphere.h"
#include <utility>

Sphere::Sphere(float r, vec3 cen, vec3 col) {
	radius = r;
	centre = cen;
	mycolour = col;
	shininess = 32;
}

Sphere::~Sphere() {
}

float Sphere::getRadius(void) {
	return radius;
}

bool Sphere::rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) {
	float t0, t1; // solutions for t if the ray intersects where t0 is enter and t1 is exit (distances)

	// geometric solution, assumes rayDirection is normalised
	vec3 L = centre - rayOrigin;
	float tca = dot(L, rayDirection);
	if(tca < 0) return false;
	float d = dot(L, L) - tca * tca;
	if(d > (radius * radius)) return false;

	float thc = sqrt(radius * radius - d);
	t0 = tca - thc;
	t1 = tca + thc;

	if(t0 > t1) std::swap(t0, t1);

	if(t0 < 0) {
		t0 = t1; // if t0 is negative, let's use t1 instead 
		if(t0 < 0) return false; // both t0 and t1 are negative 
	}

	t = t0;

	return true;
}
