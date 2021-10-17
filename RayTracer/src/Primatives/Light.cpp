#include "Primatives/Light.h"

Light::Light(LightType type) : lightType(type){
	if(type == LightType::DIRECTIONAL) {
		ambientStrength = vec3(0.1);
		diffuseStrength = vec3(0.5);
		specularStrength = vec3(0.7);
	}
	else {
		ambientStrength = vec3(0.05);
		diffuseStrength = vec3(0.8);
		specularStrength = vec3(1);

		// Intensity will reach 1% at ~36 units
		constant = 1.f;
		linear = 0.14f;
		quadratic = 0.07f;
	}

}

void Light::setDecayrate(float constant, float linear, float quadratic) {
	this->constant = constant > 0 ? constant : this->constant;
	this->linear = linear > 0 ? linear : this->linear;
	this->quadratic = quadratic > 0 ? quadratic : this->quadratic;
}

glm::vec3 Light::getAmbientStrength() {
	return ambientStrength;
}

glm::vec3 Light::getDiffuseStrength() {
	return diffuseStrength;
}

glm::vec3 Light::getSpecularStrength() {
	return specularStrength;
}

LightType Light::getLightType() {
	return lightType;
}
