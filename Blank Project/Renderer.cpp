#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include <algorithm>

Renderer::Renderer(Window& parent) :OGLRenderer(parent) 
{
	wavespeed = 0.006;
	wavetime = 0;
	treetime = 0.0;
	treespeed = 0;
	quad = Mesh::GenerateQuad();

	heightMap = new HeightMap(TEXTUREDIR"dao.jpg");

	Vector3 heightmapSize = heightMap->GetHeightMapSize();

	LoadFile();

	setShader();

	Model(heightmapSize);

	camera = new Camera(-15.0f, -135.0f, heightmapSize * Vector3(0.0f, 20.0f, 0.0f));
	camera1 = new Camera(-15.0f, 135.0f, heightmapSize * Vector3(1.0f, 6.0f, 0.0f));
	camera2 = new Camera(-15.0f, -45.0f, heightmapSize * Vector3(0.0f, 7.0f, 1.0f));
	camera3 = new Camera(-15.0f, 45.0f, heightmapSize * Vector3(1.0f, 8.0f, 1.0f));

	light = new Light(heightmapSize * Vector3(0.5f, 10.0f, 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x);
	//light1 = new Light(heightmapSize * Vector3(0.3f, 12.0f, 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	waterRotate = 0.0f;
	waterCycle = 0.0f;

	init = true;
}

Renderer::~Renderer(void) {
	delete camera;
	delete camera1;
	delete camera2;
	delete camera3;
	delete light;
	delete lightShader;
	delete reflectShader;
	delete skyboxShader;
	delete sceneshader;
	delete treeShader;
	delete heightMap;
	delete quad;
	delete rolemsh;
	delete roleanim;
	delete rolemat;
	delete role1msh;
	delete role1anim;
	delete role1mat;
	delete role2msh;
	delete role2anim;
	delete role2mat;
	delete treemsh;
	delete treemat;
	delete tree1msh;
	delete tree1mat;
	delete root;

}

//load files and textures 
void Renderer::LoadFile()
{
	treeTex = SOIL_load_OGL_texture(TEXTUREDIR"tree.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"Shazi.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthTex1 = SOIL_load_OGL_texture(TEXTUREDIR"grass.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"sandbump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"day.right.jpg", TEXTUREDIR"day.left.jpg",
		TEXTUREDIR"day.top.jpg", TEXTUREDIR"day.bottom.jpg",
		TEXTUREDIR"day.front.jpg", TEXTUREDIR"day.back.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!earthTex || !earthBump || !cubeMap || !waterTex || !waterBump || !treeTex)
	{
		return;
	}

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthTex1, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(waterBump, true);
}

//set useful shader files
void Renderer::setShader()
{

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");

	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");

	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");

	sceneshader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");

	treeShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");

	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess()  || !sceneshader->LoadSuccess() ||!treeShader->LoadSuccess()) {
		return;
	}
}

//set node and model location
void Renderer::Model(Vector3 MapSize)
{
	rolemsh = Mesh::LoadFromMeshFile("Role_T.msh");
	roleanim = new MeshAnimation("Role_T.anm");
	rolemat = new MeshMaterial("Role_T.mat");
	for (int i = 0; i < rolemsh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = rolemat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}
	root = new SceneNode();
	robot = new SceneNode(rolemsh, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	robot->SetTransform(Matrix4::Translation(MapSize * Vector3(0.2f, 2.2f, 0.5f)));
	robot->SetTransform(robot->GetTransform() * Matrix4::Rotation(30.0f, Vector3(0, 1, 0)));
	robot->SetModelScale(Vector3(150.0f, 150.0f, 150.0f));
	root->AddChild(robot);


	role1msh = Mesh::LoadFromMeshFile("Role_T.msh");
	role1anim = new MeshAnimation("Role_T.anm");
	role1mat = new MeshMaterial("Role_T.mat");
	for (int i = 0; i < role1msh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = role1mat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}
	role1 = new SceneNode(role1msh, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	role1->SetTransform(Matrix4::Translation(MapSize * Vector3(0.5f, 2.0f, 0.5f)));
	role1->SetModelScale(Vector3(150.0f, 150.0f, 150.0f));
	root->AddChild(role1);


	role2msh = Mesh::LoadFromMeshFile("Role_T.msh");
	role2anim = new MeshAnimation("Role_T.anm");
	role2mat = new MeshMaterial("Role_T.mat");
	for (int i = 0; i < role2msh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = role2mat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}
	role2 = new SceneNode(role1msh, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	role2->SetTransform(Matrix4::Translation(MapSize * Vector3(0.6f, 1.3f, 0.6f)));
	role2->SetModelScale(Vector3(150.0f, 150.0f, 150.0f));
	root->AddChild(role2);
	
	treemsh = Mesh::LoadFromMeshFile("tree_l.msh");
	treemat = new MeshMaterial("tree_l.mat");
	tree = new SceneNode(treemsh, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	tree->SetTransform(Matrix4::Translation(MapSize * Vector3(0.5f, 3.0f, 0.2f)));
	//tree->SetTransform(tree->GetTransform() * Matrix4::Rotation(-2.0f , Vector3(1,0,0)));
	tree->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	root->AddChild(tree);

	tree1msh = Mesh::LoadFromMeshFile("tree_l.msh");
	tree1mat = new MeshMaterial("tree_l.mat");
	tree1 = new SceneNode(tree1msh, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	tree1->SetTransform(Matrix4::Translation(MapSize * Vector3(0.1f, 0.3f, 0.15f)));
	//tree1->SetTransform(tree1->GetTransform() * Matrix4::Rotation(15.0f, Vector3(1, 0, 0)));
	tree1->SetModelScale(Vector3(150.0f, 150.0f, 150.0f));
	tree->AddChild(tree1);//add tree1 to be tree's child 

	monstermsh = Mesh::LoadFromMeshFile("thc6.msh");
	monsteranim = new MeshAnimation("thc6.anm");
	monstermat = new MeshMaterial("thc6.mat");
	for (int i = 0; i < monstermsh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = monstermat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures1.emplace_back(texID);
	}
	monster = new SceneNode(monstermsh, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	monster->SetTransform(Matrix4::Translation(MapSize * Vector3(0.4f, 1.0f, 0.8f)));
	monster->SetTransform(monster->GetTransform() * Matrix4::Rotation(180.0f, Vector3(0, 1, 0)));
	monster->SetModelScale(Vector3(150.0f, 150.0f, 150.0f));
	root->AddChild(monster);



	currentFrame = 0;
	frameTime = 0.0f;
	currentFrame1 = 0;
	frameTime1 = 0.0f;


}

void Renderer::UpdateScene(float dt) 
{

	camera->UpdateCamera(dt);
	camera1->UpdateCamera(dt);
	camera2->UpdateCamera(dt);
	camera3->UpdateCamera(dt);

	//light->Update(dt);


	waterRotate += dt * 0.5f;
	waterCycle += dt * 0.15f;
	wavetime += wavespeed;
	UpdateTree(dt);
	
	root->Update(dt);
	frameTime -= dt;
	frameTime1 -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % roleanim->GetFrameCount();
		frameTime += 1.0f / roleanim->GetFrameRate();
	}

	while (frameTime1 < 0.0f) {
		currentFrame1 = (currentFrame1 + 1) % monsteranim->GetFrameCount();
		frameTime1 += 1.0f / monsteranim->GetFrameRate();
	}
}

void Renderer::RenderScene() {
	glViewport(0, 0, width, height);
	viewMatrix = camera->BuildViewMatrix();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BuildNodeLists(root);
	SortNodeLists();
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawGunman(robot);
	DrawGunman(role1);
	DrawGunman(role2);
	DrawMonster(monster);
	DrawTree(tree);
	DrawTree(tree1);
	ClearNodeLists();

	if(cameraMode)
	{
		Render4Scene();
	}
	
	
}

//split screen
void Renderer::Render4Scene()
{
	glViewport(width / 2, 0, width / 2, height / 2);
	viewMatrix = camera->BuildViewMatrix();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BuildNodeLists(root);
	SortNodeLists();
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawGunman(robot);
	DrawGunman(role1);
	DrawGunman(role2);
	DrawMonster(monster);
	DrawTree(tree);
	DrawTree(tree1);
	ClearNodeLists();

	glViewport(0, 0, width / 2, height / 2);
	viewMatrix = camera1->BuildViewMatrix();
	BuildNodeLists(root);
	SortNodeLists();
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawGunman(robot);
	DrawGunman(role1);
	DrawGunman(role2);
	DrawMonster(monster);
	DrawTree(tree);
	DrawTree(tree1);
	ClearNodeLists();

	glViewport(0, height / 2, width / 2, height / 2);
	viewMatrix = camera2->BuildViewMatrix();
	BuildNodeLists(root);
	SortNodeLists();
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawGunman(robot);
	DrawGunman(role1);
	DrawGunman(role2);
	DrawMonster(monster);
	DrawTree(tree);
	DrawTree(tree1);
	ClearNodeLists();

	glViewport(width / 2, height / 2, width / 2, height / 2);
	viewMatrix = camera3->BuildViewMatrix();
	BuildNodeLists(root);
	SortNodeLists();
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawGunman(robot);
	DrawGunman(role1);
	DrawGunman(role2);
	DrawMonster(monster);
	DrawTree(tree);
	DrawTree(tree1);
	ClearNodeLists();
}

//tree will rotate and update it
void Renderer::UpdateTree(float dt )
{
	treetime += dt;
	treespeed = treetime / 2;
	if(treespeed % 2)
	{
		tree->SetTransform(tree->GetTransform() * Matrix4::Rotation(2.5f * dt, Vector3(1, 1, 0)));
		//tree1->SetTransform(tree1->GetTransform() * Matrix4::Rotation(-2.5f * dt, Vector3(1, 0, 0)));
	}
	else
	{
		tree->SetTransform(tree->GetTransform() * Matrix4::Rotation(-2.5f * dt, Vector3(1, 1, 0)));
		//tree1->SetTransform(tree1->GetTransform() * Matrix4::Rotation(2.5f * dt, Vector3(1, 0, 0)));
	}
	//tree is father of tree1,so tree1 will rotate with tree
}

void Renderer::BuildNodeLists(SceneNode* from) {
	
	Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));

	if (from->GetColour().w < 1.0f) {
		transparentNodeList.push_back(from);
	}
	else {
		nodeList.push_back(from);
	}


	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);

	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists() 
{
	transparentNodeList.clear();
	nodeList.clear();
}

/*
void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}
*/

// draw doom gunman
void Renderer::DrawGunman(SceneNode* n) {

	if (n->GetMesh())
	{
			BindShader(sceneshader);
			glUniform1i(glGetUniformLocation(sceneshader->GetProgram(), "diffuseTex"), 0);
			UpdateShaderMatrices();
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

			glUniformMatrix4fv(glGetUniformLocation(sceneshader->GetProgram(), "modelMatrix"), 1, false, model.values);

			vector<Matrix4> frameMatrices;

			const Matrix4* invBindPose = rolemsh->GetInverseBindPose();
			const Matrix4* frameData = roleanim->GetJointData(currentFrame);

			for (unsigned int i = 0; i < rolemsh->GetJointCount(); ++i) {
				frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
			}

			int j = glGetUniformLocation(sceneshader->GetProgram(), "joints");
			glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

			for (int i = 0; i < rolemsh->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, matTextures[i]);
				rolemsh->DrawSubMesh(i);
		}
	}
}

//draw doom monster
void Renderer::DrawMonster(SceneNode* n)
{
	if (n->GetMesh())
	{
		BindShader(sceneshader);
		glUniform1i(glGetUniformLocation(sceneshader->GetProgram(), "diffuseTex"), 0);
		UpdateShaderMatrices();
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(sceneshader->GetProgram(), "modelMatrix"), 1, false, model.values);

		vector<Matrix4> frameMatrices;

		const Matrix4* invBindPose = monstermsh->GetInverseBindPose();
		const Matrix4* frameData = monsteranim->GetJointData(currentFrame1);

		for (unsigned int i = 0; i < monstermsh->GetJointCount(); ++i) {
			frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
		}

		int j = glGetUniformLocation(sceneshader->GetProgram(), "joints");
		glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

		for (int i = 0; i < monstermsh->GetSubMeshCount(); ++i) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, matTextures1[i]);
			monstermsh->DrawSubMesh(i);
		}
	}
}

//draw trees
void Renderer::DrawTree(SceneNode* n)
{
	if (n->GetMesh()) {
		BindShader(treeShader);
		glUniform1i(glGetUniformLocation(sceneshader->GetProgram(), "diffuseTex"), 0);
		UpdateShaderMatrices();
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(treeShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(treeShader->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());
		glUniform1i(glGetUniformLocation(treeShader->GetProgram(), "useTexture"), 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, treeTex);
		n->Draw(*this);
	}
	
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {

	BindShader(lightShader);
	SetShaderLight(*light);

	//SetShaderLight1(*light1);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);

	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);

	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, earthTex1);
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "grassTex"), 2);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();


	heightMap->Draw();
}

void Renderer::DrawWater() {
	BindShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterBump);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	glUniform1f(glGetUniformLocation(reflectShader->GetProgram(), "time"), wavetime);

	Vector3 hSize = heightMap->GetHeightMapSize();

	modelMatrix =
		Matrix4::Translation(hSize * 0.5f) *
		Matrix4::Scale(hSize * 5.0f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	quad->Draw();
}

void Renderer::ResetCamera()
{
	Vector3 heightmapSize = heightMap->GetHeightMapSize();
	camera->SetPosition(heightmapSize * Vector3(0.0f, 20.0f, 0.0f));
	camera->SetPitch(-15.0f);
	camera->SetYaw(-135.0f);
}







