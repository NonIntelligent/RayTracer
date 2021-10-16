#pragma once
#include "Primatives/Sphere.h"
#include "Primatives/Triangle.h"
#include "Primatives/Light.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

class Scene {
	std::vector<Model*> models;
	std::vector<Light*> lights;
	// textures list?
	std::vector<std::string> objectFilePaths;

	int width = 640;
	int height = 480;

	// Image containing all of the pixels colours on screen 
	vec3** image = nullptr;

	void setupLights();
	void createRenderImage();

public:
	Scene();
	~Scene();

	void init();

	void createSphere(vec3 position, vec3 colour, float radius);
	void createTriangle(vec3 position, vec3 colour);
	void createModel(vec3 position);
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