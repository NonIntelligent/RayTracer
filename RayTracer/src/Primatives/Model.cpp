#include "Primatives/Model.h"

void Model::computeColour(Light* light, const vec3 viewDir, const IntersectData data, vec3& result) const {
	vec3 lightColour = light->colour;

	vec3 ambient = mycolour * lightColour * light->getAmbientStrength();

	vec3 toLight = normalize(light->position - data.intersectPoint);
	vec3 normal = data.normal;
	float diff = max(0.f, dot(toLight, normal));

	vec3 diffuse = mycolour * lightColour * light->getDiffuseStrength() * diff;

	vec3 reflectDir = reflect(toLight, normal);
	float spec = pow(max(0.f, dot(reflectDir, viewDir)), shininess);

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

vec3 Model::getCentre() {
	return centre;
}
