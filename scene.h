#pragma once

#include <GL/glew.h>

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "tm.h"
#include "V3.h"
#include "shadowMap.h"
#include "cubeMap.h"


#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "CGInterface.h"
// do i need this too????
// #include <GL/gl.h>

class Scene {
public:

	GUI *gui;
	FrameBuffer *fb, *fb3, *hardwareFB;
	PPC *ppc, *ppc3;
	TM *tms;
	int tmsN;

	PPC *lightPPC;
	ShadowMap *lightShadowMap;
	V3 lightPos;
	float ka;
	float es;

	int renderMode;

	bool paused = false;

	CubeMap	*environmentMap;

	float rollAngle = 0.0f;
	float phiAngle = 90.0f;
	float thetaAngle = 180.0f;

	float deltaAngleOnButtonClick = 10.0f;



	// bool wPressed = false;
	void Forward();
	void Backward();
	void Leftward();
	void Rightward();


	void LookUp();
	void LookDown();
	void LookLeft();
	void LookRight();



	Scene();
	void Render(PPC *renderPPC, FrameBuffer *renderFB);
	void Render();
	void DBG();
	void NewButton();
	void ToggleBilinear();
	void Pause();

	PPC *LoadCameraPathFromFile(char *fileName, int keyFrames);

	void TranslateLight(float x, float y, float z);
	void ChangeAmbientLight(float change);
	void SetRenderMode(int rm);
	void MultiplyES(float multiplier);

	void GenerateShadowMap(PPC *lightPPC, ShadowMap *lightShadowMap);

	void idk();
	bool filledOrWireFrame;

	// hardware stuff

	void RenderHardware();

	V3 hwClearColor;
	bool needToInitializeHardware;

	FrameBuffer *textures;
	GLuint *textureNums;
	int texturesN;

	GLuint texName;

	FrameBuffer *cubeMapTextures;
	GLuint cubeMapID;

	CGInterface *cgi;
	ShaderOneInterface *soi;
	float bwFraction;

	TM textureSquare;
	FrameBuffer *image;
	int imageSize;
	GLuint textureSquareID;
	V3 R0, R1, R2, R3;

};

extern Scene *scene;