#pragma once

#include "H:\Downloads\CSC8502 2021\nclgl\OGLRenderer.h"
#include "Camera.h"

class Renderer : public OGLRenderer
{
	public:
		Renderer(Window& parent);
		virtual ~Renderer(void);

		virtual void RenderScene();

		void SwitchToPerspective();
		void SwitchToOrthographic();

		inline void SetScale(float s)
		{
			scale = s;
		}

		inline void SetRotation(float r)
		{
			rotation = r;
		}

		inline void SetPosition(Vector3 p)
		{
			position = p;
		}

		virtual void UpdateScene(float dt);

protected:
	Mesh* triangle;
	Shader* matrixShader;
	Camera* camera=new Camera;
	float scale;
	float rotation;
	Vector3 position;
};