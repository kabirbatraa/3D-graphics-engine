#pragma once

#include <GL/glew.h>
// #include <GL/glew.h>

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "V3.h"
#include "cubeMap.h"

// class PPC;
#include "ppc.h"

class FrameBuffer : public Fl_Gl_Window {
public:

	bool isHardwareFB; // if true, then render using GPU hardware


	unsigned int *pix; // pixel array
	float *zb; // z buffer to resolve visibility

	bool mirrorTiling = false;
	bool bilinearInterpolation = false;

	int w, h;
	FrameBuffer();
	FrameBuffer(int u0, int v0, int _w, int _h);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void SetBGR(unsigned int bgr);
	void Set(int u, int v, int col);
	unsigned int Get(int u, int v); // for texture maps
	void SetGuarded(int u, int v, int col);
	void SetChecker(unsigned int col0, unsigned int col1, int csize);
	void DrawAARectangle(V3 tlc, V3 brc, unsigned int col);
	void DrawDisk(V3 center, float r, unsigned int col);
	void DrawSegment(V3 p0, V3 p1, unsigned int col);
	void DrawSegment(V3 p0, V3 p1, V3 c0, V3 c1);
	void Render3DSegment(PPC *ppc, V3 v0, V3 v1, V3 c0, V3 c1);
	void Render3DPoint(PPC *ppc, V3 p, V3 c, float psize);
	void LoadTiff(char* fname);
	void SaveAsTiff(char* fname);
	void SetZB(float zf);
	int IsCloserThenSet(float currz, int u, int v);
	// unsigned int LookUpNN(float s, float t); // renamed below
	unsigned int GetTexelColor(float normalizedS, float normalizedT);
	unsigned int GetTexelColorBilinear(float normalizedS, float normalizedT);


	unsigned int CreateColor(int r, int g, int b);
	void DrawTriangle(V3 p0, V3 p1, V3 p2, unsigned int col);
	void DrawTriangleInterpolatedColor(V3 p0, V3 p1, V3 p2, V3 c0, V3 c1, V3 c2);
	bool Sidedness(V3 edgePoint1, V3 edgePoint2, V3 point);
	void DrawTriangleWithLightAndNormals(V3 p0, V3 p1, V3 p2, PPC *ppc, V3 matColor, float ka, V3 lightPosition, V3 normal0, V3 normal1, V3 normal2);
	void DrawTriangleWithLightAndNormalsAndSpecular(V3 p0, V3 p1, V3 p2, PPC *ppc, V3 matColor, float ka, V3 lightPosition, V3 normal0, V3 normal1, V3 normal2, float es);
	void DrawTriangleWithColorsAndLightAndNormalsAndSpecular(V3 p0, V3 p1, V3 p2, PPC *ppc, V3 c0, V3 c1, V3 c2, float ka, V3 lightPosition, V3 normal0, V3 normal1, V3 normal2, float es);

	void DrawTriangleWithColorsAndLightAndNormalsAndSpecularAndShadowMap(V3 p0, V3 p1, V3 p2, PPC *ppc, V3 c0, V3 c1, V3 c2, PPC *lightPPC, ShadowMap *sm, float ka, V3 lightPosition, V3 normal0, V3 normal1, V3 normal2, float es);


	bool FastPointInTriangle(V3 A, V3 B, V3 C, V3 P);

	void RenderEnvironmentMap(CubeMap *em, PPC *ppc);

};