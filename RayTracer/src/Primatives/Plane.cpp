#include "Primatives/Plane.h"

Plane::Plane(vec3 position, vec3 colour, vec3 normal) {
	this->centre = position;
	this->mycolour = colour;
	this->planeNormal = normal;
	normalize(this->planeNormal);
}

// Using the formula from this guide
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
bool Plane::rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) {
	float denominator = dot(rayDirection, planeNormal);

	// intersects when denominator is greater than 0
	if(abs(denominator) > 0.0001f) {
		vec3 rayToPlane = centre - rayOrigin;
		t = dot(rayToPlane, planeNormal) / denominator;

		return t >= 0;
	}

	return false;
}
