#pragma once
#include "H:\Downloads\CSC8502 2021\nclgl\OGLRenderer.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);
	virtual void RenderScene();

protected:
	Mesh* triangle;
	Shader* basicShader;
};
