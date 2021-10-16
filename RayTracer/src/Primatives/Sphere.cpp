#include "Primatives/Sphere.h"
#include <utility>

Sphere::Sphere(float r, vec3 cen, vec3 col) {
	radius = r;
	center = cen;
	mycolour = col;
	shininess = 32;
}

Sphere::~Sphere() {
}

float Sphere::getRadius(void) {
	return radius;
}

vec3 Sphere::getCenter(void) {
	return center;
}

bool Sphere::rayIntersect(vec3 rayOrigin, vec3 rayDirection, float& t) {
	float t0, t1; // solutions for t if the ray intersects where t0 is enter and t1 is exit (distances)

	// geometric solution, assumes rayDirection is normalised
	vec3 L = center - rayOrigin;
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

void Sphere::computeColour(Light* light, const vec3 viewDir, const vec3 intersectPt, vec3 &result) {
	vec3 lightColour = light->colour;

	vec3 ambient = mycolour * lightColour * light->getAmbientStrength();

	vec3 ptToSource = normalize(light->position - intersectPt);
	vec3 normal = normalize(intersectPt - center);
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
