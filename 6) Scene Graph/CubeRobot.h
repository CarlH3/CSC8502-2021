#pragma once
#include"scenenode.h"

class CubeRobot : public SceneNode 
{

protected:
	SceneNode* head;
	SceneNode* leftArm;
	SceneNode* rightArm;

public:
	CubeRobot(Mesh* cube);
	~CubeRobot(void) {};
	void Update(float dt) override;
};
