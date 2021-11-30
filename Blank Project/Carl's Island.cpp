#include "../NCLGL/window.h"
#include "Renderer.h"
#include"../nclgl/Camera.h"

int main()	{
	Window w("Island!", 1920, 1080, true);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F1))
		{
			renderer.ResetCamera();
		}
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) 
		{
			Shader::ReloadAllShaders();
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1))
		{
			renderer.cameraMode = true;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2))
		{
			renderer.cameraMode = false;
		}

	}
	return 0;
}