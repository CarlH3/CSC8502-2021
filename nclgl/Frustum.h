#pragma once

#include"Plane.h"
class SceneNode;
class Matrix4;

class Frustum
{
protected:
	Plane planes[6];
public:
	Frustum() = default;
	~Frustum() = default;

	void FromMatrix(const Matrix4& mvp);
	bool InsideFrustum(SceneNode& n);
};
