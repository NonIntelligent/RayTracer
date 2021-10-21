#include "Primatives/Triangle.h"
#include <iostream>

Triangle::Triangle() {
	vertex0 = vec3(0, 6, -10);
	vertex1 = vec3(-1, 4, -10);
	vertex2 = vec3(1, 4, -10);
	mycolour = vec3(1);
	shininess = 1;
}

Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 colour, float shininess) {
	vertex0 = v0;
	vertex1 = v1;
	vertex2 = v2;
	norm0 = n0;
	norm1 = n1;
	norm2 = n2;

	mycolour = colour;
	this->shininess = shininess;
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

bool Triangle::rayIntersect(vec3 rayOrigin, vec3 rayDirection, IntersectData& data) {
	// Moller Trumbore intersection method
	vec3 v0v1 = vertex1 - vertex0; // Vertex 0 to 1
	vec3 v0v2 = vertex2 - vertex0; // Vertex 0 to 2
	vec3 pvec = cross(rayDirection, v0v2);

	float det = dot(v0v1, pvec);

#ifdef CULLING // Enabled in release mode
	// Cull back facing triangles
	if(det < epsilon) return false;
#else
	// Cull only when the ray and triangle is parallel
	if(fabsf(det) < epsilon) return false;
#endif

	float invDet = 1.f / det;

	// How far along the intersect point is on u and v
	// u and v can also be used for colour shading
	vec3 tvec = rayOrigin - vertex0;
	float u = dot(tvec, pvec) * invDet;
	if(u < epsilon || u > 1.f) return false;

	vec3 qvec = cross(tvec, v0v1);
	float v = dot(rayDirection, qvec) * invDet;
	if(v < epsilon|| u + v > 1.f) return false;

	float w = 1 - u - v;

	data.rayorigin = rayOrigin;
	data.t = dot(v0v2, qvec) * invDet;
	//data.intersectPoint = u * vertex0 + v * vertex1 + w * vertex2;
	data.intersectPoint = rayOrigin + rayDirection * data.t;
	data.normal = normalize(w * norm0 + u * norm1 + v * norm2);
	//data.normal = normalize(cross(vertex1 - vertex0, vertex2 - vertex0));
	data.model = (Model*)this;
	data.UVs.x = u;
	data.UVs.y = v;
	data.UVs.z = w;

	return data.t > epsilon;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates
bool Triangle::rayIntersectOLD(vec3 rayOrigin, vec3 rayDirection, IntersectData& data) {
	// Moller Trumbore intersection method
	vec3 v0v1 = vertex1 - vertex0; // Vertex 0 to 1
	vec3 v0v2 = vertex2 - vertex0; // Vertex 0 to 2
	vec3 normal = cross(v0v1, v0v2);
	float denom = dot(normal, normal);
	
	// Check if ray and plane are parallel 
	float angle = dot(normal, rayDirection);
	
	if (fabsf(angle) < epsilon) return false;
	
	float d = dot(normal, vertex0);
	
	float t = (dot(normal, rayOrigin) + d) / angle;
	if (t < 0) return false; // Check if triangle is behind
	
	// Compute intersection point
	vec3 p = rayOrigin + t * rayDirection;
	
	// Step 2: inside-outside test
	vec3 c;
	float u, v, w;
	
	// edge 0
	vec3 edge0 = vertex1 - vertex0;
	vec3 vp0 = p - vertex0;
	c = cross(edge0, vp0);
	if(dot(normal, c) < 0) return false;
	
	// edge 1
	vec3 edge1 = vertex2 - vertex1;
	vec3 vp1 = p - vertex1;
	c = cross(edge1, vp1);
	if(u = dot(normal, c) < 0) return false;
	
	// edge 2
	vec3 edge2 = vertex0 - vertex2;
	vec3 vp2 = p - vertex2;
	c = cross(edge2, vp2);
	if(v = dot(normal, c) < 0) return false;
	
	u /= denom;
	v /= denom;
	
	w = 1 - u - v;

	data.t = t;
	data.intersectPoint = p;
	data.normal = normalize(w * norm0 + u * norm1 + v * norm2);
	//data.normal = normal;
	data.UVs.x = u;
	data.UVs.y = v;
	data.UVs.z = w;

	return true;
}

void Triangle::computeColour(Light* light, const vec3 viewDir, const IntersectData data, vec3& result) const {
	vec3 lightColour = light->colour;

	vec3 ambient = mycolour * lightColour * light->getAmbientStrength();

	vec3 toLight = normalize(light->position - data.intersectPoint);
	vec3 normal = data.normal;
	float diff = max(0.f, dot(toLight, normal));

	vec3 diffuse = mycolour * lightColour * light->getDiffuseStrength() * diff;

	vec3 reflectDir = reflect(-toLight, normal);

	float spec = pow(max(0.f, dot(-viewDir, reflectDir)), shininess);

	vec3 specular = mycolour * lightColour * light->getSpecularStrength() * spec;

	// Calculate attenuation for point lights
	if(light->getLightType() == LightType::POINT) {
		float distance = length(toLight);
		float decayRate = (light->constant + light->linear * distance + light->quadratic * (distance * distance));
		float attenuation = 1.f / decayRate;
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
	}

	result = ambient + diffuse + specular;
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
