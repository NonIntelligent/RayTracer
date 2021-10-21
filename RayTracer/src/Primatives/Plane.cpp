#include "Primatives/Plane.h"

Plane::Plane(vec3 position, vec3 colour, vec3 normal) {
	this->centre = position;
	this->mycolour = colour;
	this->planeNormal = normal;
	normalize(this->planeNormal);
}

// Using the formula from this guide
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
bool Plane::rayIntersect(vec3 rayOrigin, vec3 rayDirection, IntersectData& data) {
	float denominator = dot(rayDirection, planeNormal);

	// intersects when denominator is greater than 0
	if(abs(denominator) > epsilon) {
		vec3 rayToPlane = centre - rayOrigin;

		data.rayorigin = rayOrigin;
		data.t = dot(rayToPlane, planeNormal) / denominator;
		data.intersect = data.t >= epsilon;
		data.intersectPoint = rayOrigin + rayDirection * data.t;
		//data.normal = normalize(data.intersectPoint - centre);
		data.normal = planeNormal;
		data.model = (Model*) this;

		return data.intersect;
	}

	return false;
}
