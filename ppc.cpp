#include "ppc.h"
#include "m33.h"

#include "tm.h"

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

PPC::PPC() {
	w = h = 0;
	C = a = b = c = V3(0, 0, 0);
}


PPC::PPC(float hfov, int _w, int _h) : w(_w), h(_h) {

	C = V3(0.0f, 0.0f, 0.0f);
	a = V3(1.0f, 0.0f, 0.0f);
	b = V3(0.0f, -1.0f, 0.0f);
	float hfovd = hfov / 180.0f * 3.1415926f; // in radians
	c = V3(-(float)w / 2.0f, (float)h / 2.0f, -(float)w / (2.0f * tan(hfovd / 2.0f)));

}

PPC::PPC(float focalLength, int _w, int _h, bool usingFocal) : w(_w), h(_h) {

	C = V3(0.0f, 0.0f, 0.0f);
	a = V3(1.0f, 0.0f, 0.0f);
	b = V3(0.0f, -1.0f, 0.0f);
	// float hfovd = hfov / 180.0f * 3.1415926f; // in radians
	// c = V3(-(float)w / 2.0f, (float)h / 2.0f, -(float)w / (2.0f * tan(hfovd / 2.0f)));
	c = GetVD()*focalLength - a * (float)w / 2.0f - b * (float)h / 2.0f;

}


int PPC::Project(V3 P, V3 &pP) {

	M33 M;
	M.SetColumn(0, a);
	M.SetColumn(1, b);
	M.SetColumn(2, c);
	V3 q = M.Inverted()*(P - C);
	if (q[2] <= 0.0f)
		return 0;

	pP[0] = q[0] / q[2];
	pP[1] = q[1] / q[2];
	pP[2] = 1.0f / q[2];
	return 1;

}

V3 PPC::GetVD() {

	return (a^b).UnitVector();

}

void PPC::Rotate(V3 aDir, float theta) {

	a = a.RotateThisVector(aDir, theta);
	b = b.RotateThisVector(aDir, theta);
	c = c.RotateThisVector(aDir, theta);

}

float PPC::GetF() {
	return c*GetVD();
}

// Cn is position, L is a point to look at, upg is up direction
void PPC::PositionAndOrient(V3 Cn, V3 L, V3 upg) {

	V3 vdn = (L - Cn).UnitVector(); // L seems to be the point we are facing toward because L - Cn gives view direction
	V3 an = (vdn ^ upg).UnitVector();
	V3 bn = vdn ^ an;
	float f = GetF(); // we preserve the camera's old focal length when positioning and orienting
	V3 cn = vdn*f - an* (float)w / 2.0f - bn* (float)h / 2.0f;

	a = an;
	b = bn;
	c = cn;
	C = Cn;

}

void PPC::Translate(V3 deltaC) {
	C = C + deltaC;
}

float PPC::GetFocalLength() { // same as GetF but better function name lol
	return c*GetVD();
}

void PPC::SetFocalLength(float newFocalLength) {
	// VD means ViewDirection
	c = GetVD()*newFocalLength - a * (float)w / 2.0f - b * (float)h / 2.0f;
}


PPC *PPC::CalculateIntermediateLinearlyInterpolatedCamera(PPC *secondCamera, float interpolationPositionFraction) {
	// int w, h; // assume w and h are constant
	// V3 a, b, c, C; must interpolate these 4 values?
	// actually no

	// what matters about a camera so that we can interpolate:
		// focal length
		// C position
		// pointing direction
		// up vector
	
	// we can get and set focal length
	// we can get and set C position
	// we can get pointing direction.. getvd, and we can set it with positionAndOrient
	// we can get up direction using negative b, and set using positionAndOrient

	// we can interpolate between focal lengths
	// we can interpolate between C 
	// how do we interpolate between pointing directions... 

	// looked it up and found a formula:
	// (1 - alpha) * start + alpha * end

	float interpolatedFocalLength = GetFocalLength() + (secondCamera->GetFocalLength() - GetFocalLength()) * interpolationPositionFraction;

	PPC *newCamera = new PPC(interpolatedFocalLength, w, h, true);
	V3 newC = C + (secondCamera->C - C) * interpolationPositionFraction; 

	V3 newPointingDirection = GetVD() * (1 - interpolationPositionFraction) + secondCamera->GetVD() * interpolationPositionFraction;
	V3 newUpDirection = (b*-1.0f) * (1 - interpolationPositionFraction) + (secondCamera->b*-1.0f) * interpolationPositionFraction;

	V3 pointToPointToward = newC + newPointingDirection;
	newCamera->PositionAndOrient(newC, pointToPointToward, newUpDirection);

	return newCamera;
	
}


void PPC::SaveCameraToTextFile(char *fileName) {

	ofstream file(fileName, ios::out);
	if (file.is_open()) {
		file << "CameraData: " << "\n";
		file << C << "\n";
		file << a << "\n";
		file << b << "\n";
		file << c << "\n";
		file.close();
	}
	else {
		cerr << "Problem with opening file: " << fileName << endl; 
	}

}

void PPC::LoadCameraFromTextFile(char *fileName) {

	ifstream file(fileName, ios::in);
	if (file.is_open()) {
		string trash;
		file >> trash >> C >> a >> b >> c;
		file.close(); //close the file object.
	}
	else {
		cerr << "Problem with opening file: " << fileName << endl; 
	}

}


TM PPC::GenerateTriangleMeshVisualization() {
	TM mesh;

	mesh.vertsN = 5;
	mesh.verts = new V3[mesh.vertsN];
	float focalLengthScaleFactor = 0.1f;

	// we need to generate the "c" lines using 
	// c = GetVD()*focalLength - a * (float)w / 2.0f - b * (float)h / 2.0f;
	// and adjust for the a and b shifts for the respective vertices

	V3 base = (GetVD()*GetFocalLength()) * focalLengthScaleFactor;
	V3 shift1 = (a * (float)w / 2.0f) * focalLengthScaleFactor;
	V3 shift2 = (b * (float)h / 2.0f) * focalLengthScaleFactor;

	mesh.verts[0] = C;
	mesh.verts[1] = C + base + shift1 + shift2; // top right
	mesh.verts[2] = C + base + shift1 - shift2; // bottom right
	mesh.verts[3] = C + base - shift1 + shift2; // top left
	mesh.verts[4] = C + base - shift1 - shift2; // bottom left

	mesh.trisN = 6; // 4 sides, one rectangle
	mesh.tris = new unsigned int[mesh.trisN*3];

	int currentTriangleNumber = 0;
	// top face
	mesh.tris[3 * currentTriangleNumber + 0] = 0; 
	mesh.tris[3 * currentTriangleNumber + 1] = 1;
	mesh.tris[3 * currentTriangleNumber + 2] = 3;
	currentTriangleNumber++;
	// right face
	mesh.tris[3 * currentTriangleNumber + 0] = 0; 
	mesh.tris[3 * currentTriangleNumber + 1] = 1;
	mesh.tris[3 * currentTriangleNumber + 2] = 2;
	currentTriangleNumber++;
	// left face
	mesh.tris[3 * currentTriangleNumber + 0] = 0;
	mesh.tris[3 * currentTriangleNumber + 1] = 3;
	mesh.tris[3 * currentTriangleNumber + 2] = 4;
	currentTriangleNumber++;
	// bottom face
	mesh.tris[3 * currentTriangleNumber + 0] = 0;
	mesh.tris[3 * currentTriangleNumber + 1] = 2;
	mesh.tris[3 * currentTriangleNumber + 2] = 4;
	currentTriangleNumber++;
	// top left top right bottom right (not in the same order)
	mesh.tris[3 * currentTriangleNumber + 0] = 1;
	mesh.tris[3 * currentTriangleNumber + 1] = 2;
	mesh.tris[3 * currentTriangleNumber + 2] = 3;
	currentTriangleNumber++;
	// top left bottom left bottom right
	mesh.tris[3 * currentTriangleNumber + 0] = 3;
	mesh.tris[3 * currentTriangleNumber + 1] = 4;
	mesh.tris[3 * currentTriangleNumber + 2] = 2;
	currentTriangleNumber++;

	mesh.cols = new V3[mesh.vertsN];
	mesh.cols[0] = V3(0.0f, 0.0f, 0.0f);
	mesh.cols[1] = 
		mesh.cols[2] = 
		mesh.cols[3] = 
		mesh.cols[4] = V3(0.0f, 0.0f, 1.0f);

	return mesh;

}


// takes in point p where x = u, y = v, and z = the depth
// this depth should actually be 1 / w
// effectively returns 3d point P
void PPC::Unproject(V3 pP, V3 &P) {

	// we divide by pP[2] because it should be storing 1 / depth
	P = C + (a*pP[0] + b*pP[1] + c) / pP[2];

}


M33 PPC::GetCameraMatrix() {
	M33 ret;
	ret.SetColumn(0, a);
	ret.SetColumn(1, b);
	ret.SetColumn(2, c);
	return ret;
}






// hardware stuff


void PPC::SetIntrinsicsHW(float nearz, float farz) {

	glViewport(0, 0, w, h);
	float scaleFactor = nearz / GetFocalLength();
	// glMatrixMode(GL_PROJECTION_MATRIX);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity(); // so we dont multiply with whats already on the stack


	// note that we are assuming the pixels are 1 by 1: 
	// w = framebuffer's w, h = framebuffer's h
	glFrustum(
		-(float)w / 2.0f * scaleFactor, 
		(float)w / 2.0f * scaleFactor, 
		-(float)h / 2.0f * scaleFactor, 
		(float)h / 2.0f * scaleFactor, 
		nearz, farz
	);

	// glMatrixMode(GL_MODELVIEW_MATRIX); // back to default
	glMatrixMode(GL_MODELVIEW); 


}



void PPC::SetExtrinsicsHW() {

	// glMatrixMode(GL_MODELVIEW_MATRIX);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity(); // so we dont multiply with whats already on the stack

	V3 L = C + GetVD() * 100.0f;

	gluLookAt(
		// eye
		C[0], C[1], C[2],
		// what to look at 
		L[0], L[1], L[2],

		-b[0], -b[1], -b[2]

	);

}

