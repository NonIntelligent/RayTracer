#pragma once
#include "Primatives/Model.h"
#include "Scene/Scene.h"

class SceneSerialiser {
public:
	void createModel(Model* model);
	void deleteModel(Model* model);

	void serialiseScene(Scene* scene);
};