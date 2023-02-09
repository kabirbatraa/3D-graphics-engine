#pragma once

#include <GL/glew.h>

#include "v3.h"
#include "tm.h"
#include "M33.h"


#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

class PPC {
public:
	V3 a, b, c, C;
	int w, h;

	PPC();
	PPC(float focalLength, int _w, int _h, bool usingFocal);
	PPC(float hfov, int _w, int _h);
	int Project(V3 P, V3 &pP);
	void Unproject(V3 pP, V3 &P);
	V3 GetVD();
	void Rotate(V3 aDir, float theta);
	void PositionAndOrient(V3 Cn, V3 L, V3 upg);
	float GetF();

	float GetFocalLength(); // same as GetF but better function name lol
	void SetFocalLength(float newFocalLength);
	void Translate(V3 deltaC);
	PPC *CalculateIntermediateLinearlyInterpolatedCamera(PPC *secondCamera, float interpolationPositionFraction);
	void SaveCameraToTextFile(char *fileName);
	void LoadCameraFromTextFile(char *fileName);
	TM GenerateTriangleMeshVisualization();

	M33 GetCameraMatrix();


	// hardware stuff

	// this is basically the hardware constructor
	void SetIntrinsicsHW(float nearz, float farz);
	void SetExtrinsicsHW();

};

