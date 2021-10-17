#include "Primatives/Triangle.h"

Triangle::Triangle() {
	vertex0 = vec3(0, 6, -10);
	vertex1 = vec3(-1, 4, -10);
	vertex2 = vec3(1, 4, -10);
	mycolour = vec3(1);
	shininess = 0;
}

Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 colour, float shininess) {
	vertex0 = v0;
	vertex1 = v1;
	vertex2 = v2;
	norm0 = n0;
	norm1 = n1;
	norm2 = n2;

	mycolour = colour;
	shininess = shininess;
}

Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 colour) {
	vertex0 = v0;
	vertex1 = v1;
	vertex2 = v2;
	mycolour = colour;
	shininess = 16;
}

Triangle::~Triangle() {

}

bool Triangle::rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) {
	// Moller Trumbore intersection method
	vec3 v0v1 = vertex1 - vertex0; // Vertex 0 to 1
	vec3 v0v2 = vertex2 - vertex0; // Vertex 0 to 2
	vec3 pvec = cross(rayDirection, v0v2);
	float det = dot(v0v1, pvec);

#ifdef CULLING // Enabled in release mode
	// Cull back facing triangles
	if(det < 0.0001f) return false;
#else
	// Cull only when the ray and triangle is parallel
	if(fabsf(det) < 0.0001f) return false;
#endif

	float invDet = 1 / det;

	// How far along the intersect point is on u and v
	// u and v can also be used for colour shading
	vec3 tvec = rayOrigin - vertex0;
	u = dot(tvec, pvec) * invDet;
	if(u < 0 || u > 1) return false;

	vec3 qvec = cross(tvec, v0v1);
	v = dot(rayDirection, qvec) * invDet;
	if(v < 0 || u + v > 1) return false;

	w = 1 - u - v;

	t = dot(v0v2, qvec) * invDet;
	return t >= 0;
}

void Triangle::computeColour(Light* light, const vec3 viewDir, const vec3 intersectPt, vec3& result) const {
	vec3 lightColour = light->colour;

	vec3 ambient = mycolour * lightColour * light->getAmbientStrength();

	vec3 ptToSource = normalize(light->position - intersectPt);
	vec3 normal = normalize(w * norm0 + u * norm1 + v * norm2);
	float diff = max(0.f, dot(ptToSource, normal));

	vec3 diffuse = mycolour * lightColour * light->getDiffuseStrength() * diff;

	vec3 reflectDir = reflect(ptToSource, normal);
	float spec = pow(max(0.f, dot(reflectDir, viewDir)), shininess);

	vec3 specular = mycolour * lightColour * light->getSpecularStrength() * spec;

	// Calculate attenuation for point lights
	if(light->getLightType() == LightType::POINT) {
		float distance = length(ptToSource);
		float decayRate = (light->constant + light->linear * distance + light->quadratic * (distance * distance));
		float attenuation = 1.f / decayRate;
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
	}

	result = ambient + diffuse + specular;
}

void Triangle::getSurfaceData(vec3 rayOrigin, vec3 rayDirection, float t, vec3& intersectPt, vec3& normVec) const {
	intersectPt = u * vertex0 + v * vertex1 + w * vertex2;
}

glm::vec3 Triangle::findAndSetCentre() {
	centre = vec3(0);

	centre.x += vertex0.x;
	centre.x += vertex1.x;
	centre.x += vertex2.x;

	centre.y += vertex0.y;
	centre.y += vertex1.y;
	centre.y += vertex2.y;

	centre.z += vertex0.z;
	centre.z += vertex1.z;
	centre.z += vertex2.z;

	centre /= 3;
	return centre;
}
