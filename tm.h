#pragma once

#include <GL/glew.h>
// #include <GL/glew.h>

#include "V3.h"
#include "shadowMap.h"
// #include "cubeMap.h"

#include "AABB.h"

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

class PPC;
class FrameBuffer;
class CubeMap;

class TM {
public:
	V3 *verts;
	int vertsN;
	V3 *cols, *normals;
	unsigned int *tris;
	int trisN;

	// texture related things:
	FrameBuffer *texture;
	GLuint textureNum;
	V3 *textureCoordinates; // aka tcs in the professor's code
	// also set these to 0 in constructor 
	void SetTexturedRectangle(float rectw, float recth);
	void SetTexturedRectangle(float rectw, float recth, float textureOffsetX, float textureOffsetY, float normalizedTextureWidth, float normalizedTextureHeight);

	// for sprites that use sprite sheets 
	void AdjustTexturedRectangle(float textureOffsetX, float textureOffsetY, float normalizedTextureWidth, float normalizedTextureHeight);


	TM() : verts(0), vertsN(0), tris(0), trisN(0), cols(0), normals(0), 
		textureCoordinates(0), texture(0) {};
	void SetOneTriangle(V3 v0, V3 v1, V3 v2);
	void SetAsAACube(V3 cc, float sideLength);
	void LoadBin(char *fname);
	void RenderAsPoints(int psize, PPC *ppc, FrameBuffer *fb);
	void RenderWireFrame(PPC *ppc, FrameBuffer *fb);
	V3 GetCenter();
	void Translate(V3 tv);
	void SetCenter(V3 newCenter);
	void Rotate(V3 aO, V3 aD, float theta);
	void Light(V3 matColor, float ka, V3 ld);
	void VisualizeVertexNormals(PPC *ppc, FrameBuffer *fb, float vlength);
	void SetUnshared(TM *tm);
	void SetAsCopy(TM *tm);
	void Explode(float t);
	void InflateFromCenter(float t, V3 center); // basically same as scale accidently

	void Scale(float scaleFactor);
	void RenderFilled(PPC *ppc, FrameBuffer *fb);
	void PointLight(V3 matColor, float ka, V3 lightPosition);
	void PointLightWithSpecular(V3 matColor, float ka, V3 lightPosition, float es, V3 eyePosition);

	void RenderFilledWithLight(PPC *ppc, FrameBuffer *fb, V3 matColor, float ka, V3 lightPosition);
	void RenderFilledWithLightAndSpecular(PPC *ppc, FrameBuffer *fb, V3 matColor, float ka, V3 lightPosition, float es);

	void RenderFilledUsingRealColorWithLightAndSpecular(PPC *ppc, FrameBuffer *fb, float ka, V3 lightPosition, float es);
	void RenderFilledUsingRealColorWithLightAndSpecularAndShadowMap(PPC *ppc, FrameBuffer *fb, PPC *lightPPC, ShadowMap *sm, float ka, V3 lightPosition, float es);

	void SetGroundPlane(V3 v0, V3 v1, V3 v2);

	void RenderToShadowMap(PPC *ppc, ShadowMap *sm);

	void RenderFilledWithTextures(PPC *ppc, FrameBuffer *fb);

	void RenderUsingCubeMap(PPC *ppc, FrameBuffer *fb, CubeMap *cubeMap);


	AABB SetAABB();

	// hardware stuff
	
	void RenderToHardware();

};

