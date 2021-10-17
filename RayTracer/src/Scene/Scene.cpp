#include "Scene/Scene.h"
#include "Primatives/Sphere.h"
#include "Primatives/Triangle.h"
#include "Primatives/Plane.h"
#include "vendor/OBJ_Loader.h"

#include <string>
#include <fstream>

vec3 convert(objl::Vector3 vector) {
	return vec3(vector.X, vector.Y, vector.Z);
}

Scene::Scene() {

}

Scene::~Scene() {

}

void Scene::setupLights() {
	createLight(vec3(5.0, 8.0, 1.0), vec3(1), vec3(-0.3, -0.3, -1.0), LightType::DIRECTIONAL);
}

void Scene::createRenderImage() {
	//create two dimensional pixel array for the image
	image = new vec3 * [width];
	for(int i = 0; i < width; i++) image[i] = new vec3[height];
}

void Scene::init() {
	createRenderImage();
	setupLights();
}

void Scene::createSphere(vec3 position, vec3 colour, float radius) {
	Model* sphere = (Model*) new Sphere(radius, position, colour);
	models.push_back(sphere);
}

void Scene::createTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 colour) {
	Model* triangle = (Model*) new Triangle(v0, v1, v2, colour);
	models.push_back(triangle);
}

void Scene::createTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 colour, float shininess) {
	Model* triangle = (Model*) new Triangle(v0, v1, v2, n0, n1, n2, colour, shininess);
	models.push_back(triangle);
}

void Scene::createPlane(vec3 position, vec3 colour, vec3 normal) {
	Model* plane = (Model*) new Plane(position, colour, normal);
	models.push_back(plane);
}

void Scene::createMesh(vec3 position, vec3 scale, vec3 colour, float shininess, const char* filepath) {
	objl::Loader loader;
	loader.LoadFile(filepath);

	int numVertices = loader.LoadedVertices.size();
	auto mesh = loader.LoadedVertices;

	for(int i = 0; i < numVertices; i += 3) {

		vec3 v0 = convert(mesh[i].Position) * scale + position;
		vec3 v1 = convert(mesh[i + 1].Position) * scale + position;
		vec3 v2 = convert(mesh[i + 2].Position) * scale + position;

		vec3 n0 = convert(mesh[i].Normal);
		vec3 n1 = convert(mesh[i + 1].Normal);
		vec3 n2 = convert(mesh[i + 2].Normal);

		Model* tri = (Model*) new Triangle(v0, v1, v2, n0, n1, n2, colour, shininess);
		
		models.push_back(tri);
	}
}

void Scene::createLight(vec3 position, vec3 colour, vec3 direction, LightType type) {
	Light* light = new Light(type);

	light->position = position;
	light->colour = colour;
	light->direction = direction;

	lights.push_back(light);
}

void Scene::generateImage() {
	// Counters for multiple files with same name
	static int filenameCount = 1;
	std::string stringCount = std::to_string(filenameCount);
	std::string name = "./test1";

	// Check if a file with that name already exists and increment counter if it does
	std::ifstream ifs = std::ifstream("./test" + stringCount + ".ppm", std::ios::binary);
	if(ifs.good()) {
		filenameCount++;
		stringCount = std::to_string(filenameCount);
		name = "./test" + stringCount;
	}

	// Write all pixels in the current frame to the file
	std::ofstream ofs = std::ofstream(name + ".ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			ofs << (unsigned char)(std::min((float)1, (float)image[x][y].x) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].y) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].z) * 255);
		}
	}
	ofs.close();
}


void Scene::deletePrimative() {

}

void Scene::deleteAllObjects() {
	for(int i = 0; i < models.size(); i++) {
		delete(models.at(i));
	}

	for(int i = 0; i < lights.size(); i++) {
		delete(lights.at(i));
	}
}

void Scene::deleteResources() {
	delete(image);
}

std::vector<Model*> Scene::getModels() {
	return models;
}

std::vector<Light*> Scene::getLights() {
	return lights;
}

const int Scene::getWidth() {
	return width;
}

const int Scene::getHeight() {
	return height;
}

glm::vec3** Scene::getPixels() {
	return image;
}

void Scene::setRenderableSize(int width, int height) {
	this->width = width;
	this->height = height;
	delete(image);
	createRenderImage();
}

