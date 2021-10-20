#pragma once
#include "Primatives/Model.h"
#include "Primatives/Light.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <algorithm>

class Scene {
	std::vector<Model*> models;
	std::vector<Light*> lights;
	std::vector<std::string> objectFilePaths;

	int width = 1280;
	int height = 720;

	// Image containing all of the pixels colours on screen 
	vec3** image = nullptr;

	void setupLights();
	void createRenderImage();

public:
	Light* mainLight = nullptr;

	Scene();
	~Scene();

	void init();

	void createSphere(vec3 position, vec3 colour, float radius);
	void createTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 colour);
	void createTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 colour, float shininess);
	void createPlane(vec3 position, vec3 colour, vec3 normal);
	void createMesh(vec3 position, vec3 scale, vec3 colour, float shininess, const char* filepath);
	void createLight(vec3 position, vec3 colour, vec3 direction, LightType type);

	void generateImage();

	void deletePrimative();
	void deleteAllObjects();
	void deleteResources();

	std::vector<Model*> getModels();
	std::vector<Light*> getLights();
	const int getWidth();
	const int getHeight();

	// Returns the array of pixel for the scene to be modified
	vec3** getPixels();

	// Sets the new size of the renderable space.
	// Also reallocates the memory needed in the image variable
	void setRenderableSize(int width, int height);
};