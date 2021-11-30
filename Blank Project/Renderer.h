#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include <Vector>;
class Camera;
class Shader;
class HeightMap;
class SceneNode;
class MeshAnimation;
class MeshMaterial;
class Light;


class Renderer :public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;

	void ResetCamera();
	bool cameraMode = false;
protected:

	

	void UpdateTree(float dt);
	void Render4Scene();

	void LoadFile();
	void setShader();
	void Model(Vector3 MapSize);

	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	//void DrawNodes();
	void DrawTree(SceneNode* n);
	void DrawGunman(SceneNode* n);
	void DrawMonster(SceneNode* n);

	Shader*			lightShader;
	Shader*			reflectShader;
	Shader*			skyboxShader;
	Shader*			sceneshader;
	Shader*			treeShader;

	HeightMap*		heightMap;
	Mesh*			quad;

	Mesh*			rolemsh;
	MeshAnimation*	roleanim;
	MeshMaterial*	rolemat;

	Mesh*			role1msh;
	MeshAnimation*	role1anim;
	MeshMaterial*	role1mat;

	Mesh*			role2msh;
	MeshAnimation*	role2anim;
	MeshMaterial*	role2mat;

	Mesh*			monstermsh;
	MeshAnimation*	monsteranim;
	MeshMaterial*	monstermat;

	Mesh*			treemsh;
	MeshMaterial*	treemat;

	Mesh*			tree1msh;
	MeshMaterial*	tree1mat;


	Light*			light;
	//Light*			light1;
	Camera*			camera;
	Camera*			camera1;
	Camera*			camera2;
	Camera*			camera3;

	GLuint cubeMap;
	GLuint waterTex;
	GLuint waterBump;
	GLuint earthTex;
	GLuint earthTex1;
	GLuint earthBump;
	GLuint texture[2];
	GLuint treeTex;
	GLuint image0;
	GLuint image1;

	vector<GLuint> matTextures;
	vector<GLuint> matTextures1;

	float waterRotate;
	float waterCycle;
	float lightRotate;
	float wavetime;
	float wavespeed;
	float treetime;
	int treespeed;

	SceneNode* root;
	SceneNode* role1;
	SceneNode* role2;
	SceneNode* tree;
	SceneNode* monster;
	SceneNode* tree1;
	SceneNode* robot;

	int currentFrame;
	float frameTime;
	int currentFrame1;
	float frameTime1;

	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	
};
