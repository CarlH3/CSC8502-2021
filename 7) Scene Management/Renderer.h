#pragma once
#include"../nclgl/OGLRenderer.h"
#include"..//nclgl/SceneNode.h"
#include"../nclgl/Frustum.h"

class Camera;
class SceneNode;
class Mesh;
class Shader;

class Renderer :public OGLRenderer
{
protected:
	void BuildNodeLists(SceneNode* from);
	void ClearNodeLists();
	SceneNode* root;
	GLuint texture;
public:
	Renderer(Window& parent);
	void UpdateScene(float msec) override;
};