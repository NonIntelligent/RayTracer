#pragma once
#include <glm/glm.hpp>
using namespace glm;

enum class LightType {
	DIRECTIONAL, POINT
};

class Light {
protected:
	vec3 ambientStrength;
	vec3 diffuseStrength;
	vec3 specularStrength;
	const LightType lightType;


public:
	Light(LightType type);
	~Light() { };

	vec3 position = vec3(0.0);
	vec3 direction = vec3(0.0, 0.0, -1.0);
	vec3 colour = vec3(0.957, 1.000, 0.980); // White sunlight value

	// Used for point/spot light intensity calculation (how fast it will dim)
	float constant = 0.f, linear = 0.f, quadratic = 0.f;

	// Default value will be used if a negative number is supplied
	void setDecayrate(float constant, float linear, float quadratic);

	vec3 getAmbientStrength();
	vec3 getDiffuseStrength();
	vec3 getSpecularStrength();
	
	LightType getLightType();
};