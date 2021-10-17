#include "Primatives/Model.h"

void Model::computeColour(Light* light, const vec3 viewDir, const vec3 intersectPt, vec3& result) const {
	vec3 lightColour = light->colour;

	vec3 ambient = mycolour * lightColour * light->getAmbientStrength();

	vec3 ptToSource = normalize(light->position - intersectPt);
	vec3 normal = normalize(intersectPt - centre);
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

void Model::getSurfaceData(vec3 rayOrigin, vec3 rayDirection, float t, vec3& intersectPt, vec3& normVec) const {
	// Calculate intersection point and normal
	intersectPt = rayOrigin + rayDirection * t;
	normVec = normalize(intersectPt - centre);
}

vec3 Model::getCentre() {
	return centre;
}
