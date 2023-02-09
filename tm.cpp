#include "tm.h"
#include "ppc.h"
#include "framebuffer.h"
#include "AABB.h"
#include "shadowMap.h"
#include "M33.h"
#include "cubeMap.h"


using namespace std;

#include <fstream>
#include <iostream>

void TM::SetAsAACube(V3 cc, float sideLength) {

	vertsN = 8;
	verts = new V3[vertsN];
	float sl2 = sideLength / 2.0f;
	verts[0] = cc + V3(-sl2, +sl2, +sl2);
	verts[1] = cc + V3(-sl2, -sl2, +sl2);
	verts[2] = cc + V3(+sl2, -sl2, +sl2);
	verts[3] = cc + V3(+sl2, +sl2, +sl2);
	verts[4] = cc + V3(-sl2, +sl2, -sl2);
	verts[5] = cc + V3(-sl2, -sl2, -sl2);
	verts[6] = cc + V3(+sl2, -sl2, -sl2);
	verts[7] = cc + V3(+sl2, +sl2, -sl2);

	trisN = 12;
	tris = new unsigned int[trisN*3];
	int tri = 0;
	tris[3 * tri + 0] = 0; // tris[0] = 0;
	tris[3 * tri + 1] = 1;
	tris[3 * tri + 2] = 2;
	tri++;
	tris[3 * tri + 0] = 2;
	tris[3 * tri + 1] = 3;
	tris[3 * tri + 2] = 0;
	tri++;

	tris[3 * tri + 0] = 3;
	tris[3 * tri + 1] = 2;
	tris[3 * tri + 2] = 6;
	tri++;
	tris[3 * tri + 0] = 6;
	tris[3 * tri + 1] = 7;
	tris[3 * tri + 2] = 3;
	tri++;

	tris[3 * tri + 0] = 7;
	tris[3 * tri + 1] = 6;
	tris[3 * tri + 2] = 5;
	tri++;
	tris[3 * tri + 0] = 5;
	tris[3 * tri + 1] = 4;
	tris[3 * tri + 2] = 7;
	tri++;

	tris[3 * tri + 0] = 4;
	tris[3 * tri + 1] = 5;
	tris[3 * tri + 2] = 1;
	tri++;
	tris[3 * tri + 0] = 1;
	tris[3 * tri + 1] = 0;
	tris[3 * tri + 2] = 4;
	tri++;


	tris[3 * tri + 0] = 1;
	tris[3 * tri + 1] = 5;
	tris[3 * tri + 2] = 6;
	tri++;
	tris[3 * tri + 0] = 6;
	tris[3 * tri + 1] = 2;
	tris[3 * tri + 2] = 1;
	tri++;

	tris[3 * tri + 0] = 4;
	tris[3 * tri + 1] = 0;
	tris[3 * tri + 2] = 3;
	tri++;
	tris[3 * tri + 0] = 3;
	tris[3 * tri + 1] = 7;
	tris[3 * tri + 2] = 4;
	tri++;

	cols = new V3[vertsN];
	cols[0] =
		cols[1] =
		cols[2] =
		cols[3] = V3(1.0f, 0.0f, 0.0f);
	cols[4] =
		cols[5] =
		cols[6] =
		cols[7] = V3(0.0f, 0.0f, 0.0f);
}

#if 0
    4      7

0     3
    5      6
1     2
#endif

void TM::LoadBin(char *fname) {

		ifstream ifs(fname, ios::binary);
		if (ifs.fail()) {
			// TCHAR NPath[MAX_PATH];
			// GetCurrentDirectory(MAX_PATH, NPath);
			// std::cout << NPath << std::endl;
			cerr << "INFO: cannot open file: " << fname << endl;
			return;
		}

		ifs.read((char*)&vertsN, sizeof(int));
		char yn;
		ifs.read(&yn, 1); // always xyz
		if (yn != 'y') {
			cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
			return;
		}
		if (verts)
			delete verts;
		verts = new V3[vertsN];

		ifs.read(&yn, 1); // cols 3 floats
		// V3 *normals = 0;
		normals = 0;
		if (cols)
			delete cols;
		cols = 0;
		if (yn == 'y') {
			cols = new V3[vertsN];
		}

		ifs.read(&yn, 1); // normals 3 floats
		if (normals)
			delete normals;
		normals = 0;
		if (yn == 'y') {
			normals = new V3[vertsN];
		}

		ifs.read(&yn, 1); // texture coordinates 2 floats
		float *tcs = 0; // don't have texture coordinates for now
		if (tcs)
			delete tcs;
		tcs = 0;
		if (yn == 'y') {
			tcs = new float[vertsN * 2];
		}


		ifs.read((char*)verts, vertsN * 3 * sizeof(float)); // load verts

		if (cols) {
			ifs.read((char*)cols, vertsN * 3 * sizeof(float)); // load cols
		}

		if (normals)
			ifs.read((char*)normals, vertsN * 3 * sizeof(float)); // load normals

		if (tcs)
			ifs.read((char*)tcs, vertsN * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&trisN, sizeof(int));
	if (tris)
		delete tris;
	tris = new unsigned int[trisN * 3];
	ifs.read((char*)tris, trisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	cerr << "      xyz " << ((cols) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

}


void TM::RenderAsPoints(int psize, PPC *ppc, FrameBuffer *fb) {

	for (int vi = 0; vi < vertsN; vi++) {
		V3 pp;
		if (!ppc->Project(verts[vi], pp))
			continue;
		fb->DrawDisk(pp, (float)psize, 0xFF000000);
	}

}

void TM::RenderWireFrame(PPC *ppc, FrameBuffer *fb) {

	for (int tri = 0; tri < trisN; tri++) {
		for (int ei = 0; ei < 3; ei++) {
			V3 v0 = verts[tris[3 * tri + ei]];
			V3 v1 = verts[tris[3 * tri + (ei + 1) % 3]];
			V3 c0 = cols[tris[3 * tri + ei]];
			V3 c1 = cols[tris[3 * tri + (ei + 1) % 3]];
			V3 pv0, pv1;
			if (!ppc->Project(v0, pv0))
				continue;
			if (!ppc->Project(v1, pv1))
				continue;
			fb->DrawSegment(pv0, pv1, c0, c1);
		}
	}

}

V3 TM::GetCenter() {

	V3 ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < vertsN; vi++)
		ret = ret + verts[vi];
	ret = ret / (float)vertsN;
	return ret;

}

void TM::Rotate(V3 aO, V3 aD, float theta) {

	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi].RotateThisPoint(aO, aD, theta);
	}

}


void TM::Translate(V3 tv) {

	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi] + tv;

}

void TM::SetCenter(V3 newCenter) {

	V3 center = GetCenter();
	V3 tv = newCenter - center; // tv means translation vector
	Translate(tv);

}

// scale about center of mass of object
void TM::Scale(float scaleFactor) {

	V3 center = GetCenter();
	SetCenter(V3(0, 0, 0));

	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi] * scaleFactor;
	
	Translate(center);
}


// ka is proportion of ambient light, ld is the direction of light
void TM::Light(V3 matColor, float ka, V3 ld) {

	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}
	for (int vi = 0; vi < vertsN; vi++) {
		cols[vi] = verts[vi].Light(matColor, ka, ld, normals[vi]);
	}

}

void TM::VisualizeVertexNormals(PPC *ppc, FrameBuffer *fb, float vlength) {

	if (!normals)
		return;
	for (int vi = 0; vi < vertsN; vi++) {
		fb->Render3DSegment(ppc, verts[vi], verts[vi] + normals[vi] * vlength,
			V3(0.0f, 0.0f, 0.0f), V3(1.0f, 0.0f, 0.0f));
	}
	fb->redraw();

}

void TM::SetUnshared(TM *tm) {

	trisN = tm->trisN;
	vertsN = tm->trisN * 3;
	verts = new V3[vertsN];
	tris = new unsigned int[trisN*3];
	if (tm->cols)
		cols = new V3[vertsN];
	if (tm->normals)
		normals = new V3[vertsN];

	for (int ti = 0; ti < tm->trisN; ti++) {
		verts[3 * ti + 0] = tm->verts[tm->tris[ti * 3 + 0]];
		verts[3 * ti + 1] = tm->verts[tm->tris[ti * 3 + 1]];
		verts[3 * ti + 2] = tm->verts[tm->tris[ti * 3 + 2]];
		if (cols) {
			cols[3 * ti + 0] = tm->cols[tm->tris[ti * 3 + 0]];
			cols[3 * ti + 1] = tm->cols[tm->tris[ti * 3 + 1]];
			cols[3 * ti + 2] = tm->cols[tm->tris[ti * 3 + 2]];
		}
		if (normals) {
			normals[3 * ti + 0] = tm->normals[tm->tris[ti * 3 + 0]];
			normals[3 * ti + 1] = tm->normals[tm->tris[ti * 3 + 1]];
			normals[3 * ti + 2] = tm->normals[tm->tris[ti * 3 + 2]];
		}
		tris[3 * ti + 0] = 3 * ti + 0;
		tris[3 * ti + 1] = 3 * ti + 1;
		tris[3 * ti + 2] = 3 * ti + 2;
	}


}


void TM::SetAsCopy(TM *tm) {

	vertsN = tm->vertsN;
	trisN = tm->trisN;
	verts = new V3[vertsN];
	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = tm->verts[vi];
	tris = new unsigned int[3 * trisN];
	for (int ti = 0; ti < trisN*3; ti++)
		tris[ti] = tm->tris[ti];
	if (tm->cols) {
		cols = new V3[vertsN];
		for (int vi = 0; vi < vertsN; vi++)
			cols[vi] = tm->cols[vi];
	}
	if (tm->normals) {
		normals = new V3[vertsN];
		for (int vi = 0; vi < vertsN; vi++)
			normals[vi] = tm->normals[vi];
	}

}

void TM::Explode(float t) {
	for (int ti = 0; ti < trisN; ti++) {
		unsigned int inds[3];
		inds[0] = tris[3 * ti + 0];
		inds[1] = tris[3 * ti + 1];
		inds[2] = tris[3 * ti + 2];
		// v = (v1-v0) ^ (v2-v0)
		V3 n = (verts[inds[1]] - verts[inds[0]]) ^
			(verts[inds[2]] - verts[inds[0]]);
		n = n.UnitVector();
		verts[inds[0]] = verts[inds[0]] + n*t;
		verts[inds[1]] = verts[inds[1]] + n*t;
		verts[inds[2]] = verts[inds[2]] + n*t;
	}
}

void TM::InflateFromCenter(float t, V3 center) {
	for (int ti = 0; ti < trisN; ti++) {
		unsigned int inds[3];
		inds[0] = tris[3 * ti + 0];
		inds[1] = tris[3 * ti + 1];
		inds[2] = tris[3 * ti + 2];
		// v = (v1-v0) ^ (v2-v0)

		// V3 n = (verts[inds[1]] - verts[inds[0]]) ^
		// 	(verts[inds[2]] - verts[inds[0]]);
		// n = n.UnitVector();

		// verts[inds[0]] = verts[inds[0]] + n*t;
		// verts[inds[1]] = verts[inds[1]] + n*t;
		// verts[inds[2]] = verts[inds[2]] + n*t;

		// V3 center = GetCenter();

		for (int i = 0; i < 3; i++) {
			V3 vertex = verts[inds[i]];
			V3 n = (vertex - center).UnitVector();
			verts[inds[i]] = verts[inds[i]] + n*t;
		}

	}
}

void TM::RenderFilled(PPC *ppc, FrameBuffer *fb) {
	for (int tri = 0; tri < trisN; tri++) {
		V3 v0 = verts[tris[3 * tri + 0]];
		V3 v1 = verts[tris[3 * tri + 1]];
		V3 v2 = verts[tris[3 * tri + 2]];

		V3 pv0, pv1, pv2;
		if (!ppc->Project(v0, pv0))
			return;
		if (!ppc->Project(v1, pv1))
			return;
		if (!ppc->Project(v2, pv2))
			return;

		
		V3 c0 = cols[tris[3 * tri + 0]];
		V3 c1 = cols[tris[3 * tri + 1]];
		V3 c2 = cols[tris[3 * tri + 2]];
		
		fb->DrawTriangleInterpolatedColor(pv0, pv1, pv2, c0, c1, c2);
	}
}

void TM::RenderToShadowMap(PPC *ppc, ShadowMap *sm) {
	for (int tri = 0; tri < trisN; tri++) {
		V3 v0 = verts[tris[3 * tri + 0]];
		V3 v1 = verts[tris[3 * tri + 1]];
		V3 v2 = verts[tris[3 * tri + 2]];

		V3 pv0, pv1, pv2;
		if (!ppc->Project(v0, pv0))
			return;
		if (!ppc->Project(v1, pv1))
			return;
		if (!ppc->Project(v2, pv2))
			return;

		sm->AddTriangle(pv0, pv1, pv2);
	}
}




// ka is proportion of ambient light, 
void TM::PointLight(V3 matColor, float ka, V3 lightPosition) {

	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}
	for (int vi = 0; vi < vertsN; vi++) {
		V3 ld = (verts[vi] - lightPosition).UnitVector();
		cols[vi] = verts[vi].Light(matColor, ka, ld, normals[vi]);
	}

}

void TM::PointLightWithSpecular(V3 matColor, float ka, V3 lightPosition, float es, V3 eyePosition) {

	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}
	for (int vi = 0; vi < vertsN; vi++) {
		V3 ld = (verts[vi] - lightPosition).UnitVector();
		cols[vi] = verts[vi].LightWithSpecular(matColor, ka, ld, normals[vi], es, eyePosition, lightPosition, verts[vi]);
		// V3 matColor, float ka, V3 ld, V3 norm, float es, V3 eyePosition, V3 lightPosition, V3 thisPointPosition
	}

}




void TM::RenderFilledWithLight(PPC *ppc, FrameBuffer *fb, V3 matColor, float ka, V3 lightPosition) {

	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}

	for (int tri = 0; tri < trisN; tri++) {
		V3 v0 = verts[tris[3 * tri + 0]];
		V3 v1 = verts[tris[3 * tri + 1]];
		V3 v2 = verts[tris[3 * tri + 2]];

		V3 pv0, pv1, pv2;
		if (!ppc->Project(v0, pv0))
			return;
		if (!ppc->Project(v1, pv1))
			return;
		if (!ppc->Project(v2, pv2))
			return;

		
		V3 normal0 = normals[tris[3 * tri + 0]];
		V3 normal1 = normals[tris[3 * tri + 1]];
		V3 normal2 = normals[tris[3 * tri + 2]];
		
		fb->DrawTriangleWithLightAndNormals(pv0, pv1, pv2, ppc, matColor, ka, lightPosition, normal0, normal1, normal2);
	}
}


void TM::RenderFilledWithLightAndSpecular(PPC *ppc, FrameBuffer *fb, V3 matColor, float ka, V3 lightPosition, float es) {

	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}

	for (int tri = 0; tri < trisN; tri++) {
		V3 v0 = verts[tris[3 * tri + 0]];
		V3 v1 = verts[tris[3 * tri + 1]];
		V3 v2 = verts[tris[3 * tri + 2]];

		V3 pv0, pv1, pv2;
		if (!ppc->Project(v0, pv0))
			return;
		if (!ppc->Project(v1, pv1))
			return;
		if (!ppc->Project(v2, pv2))
			return;

		
		V3 normal0 = normals[tris[3 * tri + 0]];
		V3 normal1 = normals[tris[3 * tri + 1]];
		V3 normal2 = normals[tris[3 * tri + 2]];
		
		fb->DrawTriangleWithLightAndNormalsAndSpecular(pv0, pv1, pv2, ppc, matColor, ka, lightPosition, normal0, normal1, normal2, es);
	}
}

void TM::RenderFilledUsingRealColorWithLightAndSpecular(PPC *ppc, FrameBuffer *fb, float ka, V3 lightPosition, float es) {

	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}

	for (int tri = 0; tri < trisN; tri++) {
		V3 v0 = verts[tris[3 * tri + 0]];
		V3 v1 = verts[tris[3 * tri + 1]];
		V3 v2 = verts[tris[3 * tri + 2]];

		V3 pv0, pv1, pv2;
		if (!ppc->Project(v0, pv0))
			return;
		if (!ppc->Project(v1, pv1))
			return;
		if (!ppc->Project(v2, pv2))
			return;

		
		V3 normal0 = normals[tris[3 * tri + 0]];
		V3 normal1 = normals[tris[3 * tri + 1]];
		V3 normal2 = normals[tris[3 * tri + 2]];

		V3 c0 = cols[tris[3 * tri + 0]];
		V3 c1 = cols[tris[3 * tri + 1]];
		V3 c2 = cols[tris[3 * tri + 2]];
		
		fb->DrawTriangleWithColorsAndLightAndNormalsAndSpecular(pv0, pv1, pv2, ppc, c0, c1, c2, ka, lightPosition, normal0, normal1, normal2, es);
	}
}

void TM::RenderFilledUsingRealColorWithLightAndSpecularAndShadowMap(PPC *ppc, FrameBuffer *fb, PPC *lightPPC, ShadowMap *sm, float ka, V3 lightPosition, float es) {

	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}

	// go through all the triangles in this triangle mesh
	for (int tri = 0; tri < trisN; tri++) {
		// get the 3 vertices
		V3 v0 = verts[tris[3 * tri + 0]];
		V3 v1 = verts[tris[3 * tri + 1]];
		V3 v2 = verts[tris[3 * tri + 2]];

		// project the 3 vertices onto the camera to get the (u,v) values for the framebuffer
		// If any of them are behind the camera, skip this triangle
		V3 pv0, pv1, pv2;
		if (!ppc->Project(v0, pv0))
			continue;
		if (!ppc->Project(v1, pv1))
			continue;
		if (!ppc->Project(v2, pv2))
			continue;

		// create a 2d axis aligned bounding box of the triangle
		// using the projected points 
		// if the box is completely outside the framebuffer, do not render the triangle
		AABB aabb(pv0);
		aabb.AddPoint(pv1);
		aabb.AddPoint(pv2);
		if (!aabb.ClipWithFrame(fb->w, fb->h))
			continue;

		// get the normals of each vertex
		V3 normal0 = normals[tris[3 * tri + 0]];
		V3 normal1 = normals[tris[3 * tri + 1]];
		V3 normal2 = normals[tris[3 * tri + 2]];

		// get the colors of each vertex
		V3 c0 = cols[tris[3 * tri + 0]];
		V3 c1 = cols[tris[3 * tri + 1]];
		V3 c2 = cols[tris[3 * tri + 2]];

		// pass into the function that draws a triangle on the frame buffer

		// suggestion from the teacher: 
		// do all of that code in this function instead of moving it all to another function and passing all of the data 1 variable at a time
		
		// fb->DrawTriangleWithColorsAndLightAndNormalsAndSpecular(pv0, pv1, pv2, ppc, c0, c1, c2, ka, lightPosition, normal0, normal1, normal2, es);
		fb->DrawTriangleWithColorsAndLightAndNormalsAndSpecularAndShadowMap(pv0, pv1, pv2, ppc, c0, c1, c2, lightPPC, sm, ka, lightPosition, normal0, normal1, normal2, es);
	}
}





// i want this to be the new renderfilled
// it no longer calls a complex function in fb, but instead 
// does interpolation and setting of pixels here
// but calling fb-> functions
void TM::RenderFilledWithTextures(PPC *ppc, FrameBuffer *fb) {
	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}

	// go through all the triangles in this triangle mesh
	for (int tri = 0; tri < trisN; tri++) {
		// get the 3 vertices
			V3 vertex0 = verts[tris[3 * tri + 0]];
		V3 vertex1 = verts[tris[3 * tri + 1]];
		V3 vertex2 = verts[tris[3 * tri + 2]];

		// project the 3 vertices onto the camera to get the (u,v) values for the framebuffer
		// If any of them are behind the camera, skip this triangle
		V3 p0, p1, p2;
		if (!ppc->Project(vertex0, p0))
			continue;
		if (!ppc->Project(vertex1, p1))
			continue;
		if (!ppc->Project(vertex2, p2))
			continue;

		// create a 2d axis aligned bounding box of the triangle
		// using the projected points 
		// if the box is completely outside the framebuffer, do not render the triangle
		AABB aabb(p0);
		aabb.AddPoint(p1);
		aabb.AddPoint(p2);
		if (!aabb.ClipWithFrame(fb->w, fb->h))
			continue;

		// get the normals of each vertex
		V3 normal0 = normals[tris[3 * tri + 0]];
		V3 normal1 = normals[tris[3 * tri + 1]];
		V3 normal2 = normals[tris[3 * tri + 2]];

		// get the colors of each vertex
		V3 c0 = cols[tris[3 * tri + 0]];
		V3 c1 = cols[tris[3 * tri + 1]];
		V3 c2 = cols[tris[3 * tri + 2]];


		// get the texture coordinates
		V3 tc0, tc1, tc2;
		if (textureCoordinates) {
			tc0 = textureCoordinates[tris[3 * tri + 0]];
			tc1 = textureCoordinates[tris[3 * tri + 1]];
			tc2 = textureCoordinates[tris[3 * tri + 2]];
		}




		// we can use the bounding box to get explicit up down left right for our for loop
		// TODO
		int up = min(min(p0[1], p1[1]), p2[1]);
		int down = max(max(p0[1], p1[1]), p2[1]);
		int left = min(min(p0[0], p1[0]), p2[0]);
		int right = max(max(p0[0], p1[0]), p2[0]);

		for (int i = left; i <= right; i++) {
			for (int j = up; j <= down; j++) {
				V3 currentPoint(i, j, 0);

				if (fb->FastPointInTriangle(p0, p1, p2, currentPoint)) {
					// screenspace interpolation to get pixel's color here

					// all of this interpolation code could have been much more concise if
					// i used matrices instead of multiplying everything out as vectors
					// but i tried it once and didn't realize to transpose the matrices
					// so i got junk values and gave up and restarted

					float u = i;
					float v = j;

					float u0 = p0[0];
					float v0 = p0[1];

					float u1 = p1[0];
					float v1 = p1[1];

					float u2 = p2[0];
					float v2 = p2[1];

					M33 leftMatrix;
					leftMatrix[0] = V3(u0, v0, 1);
					leftMatrix[1] = V3(u1, v1, 1);
					leftMatrix[2] = V3(u2, v2, 1);

					M33 invertedLeftMatrix = leftMatrix.Inverted();

					// V3 rValues(c0[0], c1[0], c2[0]);
					// V3 gValues(c0[1], c1[1], c2[1]);
					// V3 bValues(c0[2], c1[2], c2[2]);
					V3 normalxValues(normal0[0], normal1[0], normal2[0]);
					V3 normalyValues(normal0[1], normal1[1], normal2[1]);
					V3 normalzValues(normal0[2], normal1[2], normal2[2]);
					// let ood mean one over depth
					V3 oodValues(p0[2], p1[2], p2[2]);

					// V3 abcForR = invertedLeftMatrix * rValues;
					// V3 abcForG = invertedLeftMatrix * gValues;
					// V3 abcForB = invertedLeftMatrix * bValues;
					V3 abcFornormalx = invertedLeftMatrix * normalxValues;
					V3 abcFornormaly = invertedLeftMatrix * normalyValues;
					V3 abcFornormalz = invertedLeftMatrix * normalzValues;
					V3 abcForOod = invertedLeftMatrix * oodValues;

					// float rAtPoint = abcForR[0] * u + abcForR[1] * v + abcForR[2];
					// float gAtPoint = abcForG[0] * u + abcForG[1] * v + abcForG[2];
					// float bAtPoint = abcForB[0] * u + abcForB[1] * v + abcForB[2];
					float normalXAtPoint = abcFornormalx[0] * u + abcFornormalx[1] * v + abcFornormalx[2];
					float normalYAtPoint = abcFornormaly[0] * u + abcFornormaly[1] * v + abcFornormaly[2];
					float normalZAtPoint = abcFornormalz[0] * u + abcFornormalz[1] * v + abcFornormalz[2];
					float oodAtPoint = abcForOod[0] * u + abcForOod[1] * v + abcForOod[2];

					V3 finalNormal = V3(normalXAtPoint, normalYAtPoint, normalZAtPoint).UnitVector();

					// just color
					V3 colorXValues(c0[0], c1[0], c2[0]);
					V3 colorYValues(c0[1], c1[1], c2[1]);
					V3 colorZValues(c0[2], c1[2], c2[2]);
					V3 abcForColorx = invertedLeftMatrix * colorXValues;
					V3 abcForColory = invertedLeftMatrix * colorYValues;
					V3 abcForColorz = invertedLeftMatrix * colorZValues;
					float colorXAtP = abcForColorx[0] * u + abcForColorx[1] * v + abcForColorx[2];
					float colorYAtP = abcForColory[0] * u + abcForColory[1] * v + abcForColory[2];
					float colorZAtP = abcForColorz[0] * u + abcForColorz[1] * v + abcForColorz[2];
					V3 finalMaterialColor = V3(colorXAtP, colorYAtP, colorZAtP).UnitVector();
					V3 matColor = finalMaterialColor;

					/* commenting out anything to do with lighting so we just get interpolated color
					

					// unproject this current point to get the direction of the light
					V3 currentPoint(u, v, oodAtPoint);
					V3 unprojectedPoint;
					ppc->Unproject(currentPoint, unprojectedPoint);
					V3 thisPointPosition = unprojectedPoint;

					// light direction
					V3 ld = (unprojectedPoint - lightPosition).UnitVector();
					V3 eyePosition = ppc->C;
					V3 colorWithLightingAndSpecular = unprojectedPoint.LightWithSpecular(matColor, ka, ld, finalNormal, es, eyePosition, lightPosition, thisPointPosition);

					V3 colorWithOnlyAmbient = matColor * ka;

					bool inLight = false;
					// we need to figure out if the object is in the light or not using the shadow map of the light

					// get shadow map and light ppc
					// sm and lightPPC 

					// get the current unprojected point and project it using the light ppc to get u and v and z 
					// unprojectedPoint
					V3 pointProjectedOntoLight;
					lightPPC->Project(unprojectedPoint, pointProjectedOntoLight);

					// get the closest value in the z buffer and 
					int lightU = (int)(pointProjectedOntoLight[0]+0.5f);
					int lightV = (int)(pointProjectedOntoLight[1]+0.5f);
					float zBufferValue = sm->GetZB(lightU, lightV);
					
					// if the z values are close by an epsilon, then it is in the light
					// otherwise the z is much farther back and it is in the shadow
					float epsilon = 0.5f;
					inLight = abs(zBufferValue - pointProjectedOntoLight[2]) < epsilon; 
					


					V3 colorToUse = inLight ? colorWithLightingAndSpecular : colorWithOnlyAmbient;
					*/
					V3 colorToUse = finalMaterialColor; // interpolated color


					// in order to get the correct color of the current (u, v)
					// we need to first get the interpolated texture coordinate
					// and then GetTexelColor(texture coordinates) to get the correct final color

					// incorrect screenspace interpolation: 
					// M33 textureCoordinateMatrix;
					// textureCoordinateMatrix[0] = tc0;
					// textureCoordinateMatrix[1] = tc1;
					// textureCoordinateMatrix[2] = tc2;
					// M33 leTextureCordinates = (invertedLeftMatrix * textureCoordinateMatrix).Transposed();

					// V3 pixelUV(i, j, 1.0f);
					// V3 currentTextureCordinates = leTextureCordinates * pixelUV;

					// slides
					// variables we are interpolating between: tc0, tc1, tc2 
					// vertices: vertex0, 1, 2
					// camera: ppc->C
					// left matrix = [V1 - C, V2 - C, V3 - C] (these are columns)
					// M33 leftMatrix; // left matrix already exists so we can use the same variable
					leftMatrix.SetColumn(0, vertex0 - ppc->C);
					leftMatrix.SetColumn(1, vertex1 - ppc->C);
					leftMatrix.SetColumn(2, vertex2 - ppc->C);
					// M33 invertedLeftMatrix = leftMatrix.Inverted();
					invertedLeftMatrix = leftMatrix.Inverted();

					M33 abcCameraMatrix;
					abcCameraMatrix.SetColumn(0, ppc->a);
					abcCameraMatrix.SetColumn(1, ppc->b);
					abcCameraMatrix.SetColumn(2, ppc->c);

					// q matrix is inverted left * camera abc
					M33 qMatrix;
					qMatrix = invertedLeftMatrix * abcCameraMatrix;

					// you should consider rewriting the 3 variables below using matrix multiplication instead  

					// get demoninator of linear equation division
					float denominator = (qMatrix[0][0] + qMatrix[1][0] + qMatrix[2][0]) * u
						+ (qMatrix[0][1] + qMatrix[1][1] + qMatrix[2][1]) * v
						+ (qMatrix[0][2] + qMatrix[1][2] + qMatrix[2][2]) * 1;
					// yeah this couldve been a really short line but eh

					// numerator is different for each parameter we are interpolating
					// we are interpolating s and also t = tc0[0] vs tc0[1]
					// tc0, tc1, and tc2 become r1 r2 r3, except twice
					float numeratorS = (qMatrix[0][0]*tc0[0] + qMatrix[1][0]*tc1[0] + qMatrix[2][0]*tc2[0]) * u
						+ (qMatrix[0][1]*tc0[0] + qMatrix[1][1]*tc1[0] + qMatrix[2][1]*tc2[0]) * v
						+ (qMatrix[0][2]*tc0[0] + qMatrix[1][2]*tc1[0] + qMatrix[2][2]*tc2[0]) * 1;

					float numeratorT = (qMatrix[0][0]*tc0[1] + qMatrix[1][0]*tc1[1] + qMatrix[2][0]*tc2[1]) * u
						+ (qMatrix[0][1]*tc0[1] + qMatrix[1][1]*tc1[1] + qMatrix[2][1]*tc2[1]) * v
						+ (qMatrix[0][2]*tc0[1] + qMatrix[1][2]*tc1[1] + qMatrix[2][2]*tc2[1]) * 1;

					float interpolatedS = numeratorS / denominator;
					float interpolatedT = numeratorT / denominator;


					if (fb->IsCloserThenSet(oodAtPoint, u, v)) {

						if (texture) {
							if (texture->bilinearInterpolation) {
								fb->SetGuarded(i, j, texture->GetTexelColorBilinear(interpolatedS, interpolatedT));
							}
							else {
								fb->SetGuarded(i, j, texture->GetTexelColor(interpolatedS, interpolatedT));
							}
						}
						else {
							fb->SetGuarded(i, j, colorToUse.GetColor());
						}
					}
					// if not closer, then dont set
				}
			}
		}










		
		// fb->DrawTriangleWithColorsAndLightAndNormalsAndSpecularAndShadowMap(pv0, pv1, pv2, ppc, c0, c1, c2, lightPPC, sm, ka, lightPosition, normal0, normal1, normal2, es);
	}
}



void TM::RenderUsingCubeMap(PPC *ppc, FrameBuffer *fb, CubeMap *cubeMap) {
	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}

	// go through all the triangles in this triangle mesh
	for (int tri = 0; tri < trisN; tri++) {
		// get the 3 vertices
		V3 vertex0 = verts[tris[3 * tri + 0]];
		V3 vertex1 = verts[tris[3 * tri + 1]];
		V3 vertex2 = verts[tris[3 * tri + 2]];

		// project the 3 vertices onto the camera to get the (u,v) values for the framebuffer
		// If any of them are behind the camera, skip this triangle
		V3 p0, p1, p2;
		if (!ppc->Project(vertex0, p0))
			continue;
		if (!ppc->Project(vertex1, p1))
			continue;
		if (!ppc->Project(vertex2, p2))
			continue;

		// create a 2d axis aligned bounding box of the triangle
		// using the projected points 
		// if the box is completely outside the framebuffer, do not render the triangle
		AABB aabb(p0);
		aabb.AddPoint(p1);
		aabb.AddPoint(p2);
		if (!aabb.ClipWithFrame(fb->w, fb->h))
			continue;

		// get the normals of each vertex
		V3 normal0 = normals[tris[3 * tri + 0]];
		V3 normal1 = normals[tris[3 * tri + 1]];
		V3 normal2 = normals[tris[3 * tri + 2]];


		// we can use the bounding box to get explicit up down left right for our for loop
		int up = min(min(p0[1], p1[1]), p2[1]);
		int down = max(max(p0[1], p1[1]), p2[1]);
		int left = min(min(p0[0], p1[0]), p2[0]);
		int right = max(max(p0[0], p1[0]), p2[0]);

		for (int i = left; i <= right; i++) {
			for (int j = up; j <= down; j++) {
				V3 currentPoint(i, j, 0);

				if (fb->FastPointInTriangle(p0, p1, p2, currentPoint)) {

					float u = i;
					float v = j;

					float u0 = p0[0];
					float v0 = p0[1];

					float u1 = p1[0];
					float v1 = p1[1];

					float u2 = p2[0];
					float v2 = p2[1];

					M33 leftMatrix;
					leftMatrix[0] = V3(u0, v0, 1);
					leftMatrix[1] = V3(u1, v1, 1);
					leftMatrix[2] = V3(u2, v2, 1);

					M33 invertedLeftMatrix = leftMatrix.Inverted();

					V3 normalxValues(normal0[0], normal1[0], normal2[0]);
					V3 normalyValues(normal0[1], normal1[1], normal2[1]);
					V3 normalzValues(normal0[2], normal1[2], normal2[2]);
					// let ood mean one over depth
					V3 oodValues(p0[2], p1[2], p2[2]);

					V3 abcFornormalx = invertedLeftMatrix * normalxValues;
					V3 abcFornormaly = invertedLeftMatrix * normalyValues;
					V3 abcFornormalz = invertedLeftMatrix * normalzValues;
					V3 abcForOod = invertedLeftMatrix * oodValues;

					float normalXAtPoint = abcFornormalx[0] * u + abcFornormalx[1] * v + abcFornormalx[2];
					float normalYAtPoint = abcFornormaly[0] * u + abcFornormaly[1] * v + abcFornormaly[2];
					float normalZAtPoint = abcFornormalz[0] * u + abcFornormalz[1] * v + abcFornormalz[2];
					float oodAtPoint = abcForOod[0] * u + abcForOod[1] * v + abcForOod[2];

					V3 finalNormal = V3(normalXAtPoint, normalYAtPoint, normalZAtPoint).UnitVector();


					V3 n = finalNormal;
					V3 eyePosition = ppc->C;
					V3 currentPoint(u, v, oodAtPoint);
					V3 unprojectedPoint; ppc->Unproject(currentPoint, unprojectedPoint);
					V3 thisPointPosition = unprojectedPoint;


					V3 eyeToPoint = thisPointPosition - eyePosition;
					V3 verticalComponent = n * (eyeToPoint * n); // note that this is going in the oposite direction of the normal
					V3 reflectedVector = eyeToPoint - verticalComponent * 2;
					reflectedVector = reflectedVector.UnitVector();

					unsigned int colorFromCubeMap = cubeMap->GetColor(reflectedVector);

					if (fb->IsCloserThenSet(oodAtPoint, u, v)) {
						fb->SetGuarded(i, j, colorFromCubeMap);
					}
					// if not closer, then dont set
				}
			}
		} // end of double for loop

	} // end of loop iterating through triangles
}

















void TM::SetOneTriangle(V3 v0, V3 v1, V3 v2) {

	vertsN = 3;
	trisN = 1;
	verts = new V3[vertsN];
	cols = new V3[vertsN];
	normals = new V3[vertsN];

	verts[0] = v0;
	verts[1] = v1;
	verts[2] = v2;
	cols[0] = V3(0.0f, 0.0f, 0.0f);
	cols[1] = cols[0];
	cols[2] = cols[0];
	V3 n = ((v1 - v0) ^ (v2 - v0)).UnitVector();
	normals[0] =
		normals[1] =
		normals[2] = n;

	tris = new unsigned int[3 * trisN];
	tris[0] = 0;
	tris[1] = 1;
	tris[2] = 2;
}


void TM::SetGroundPlane(V3 v0, V3 v1, V3 v2) {

	vertsN = 3;
	trisN = 1;
	verts = new V3[vertsN];
	cols = new V3[vertsN];
	normals = new V3[vertsN];

	verts[0] = v0;
	verts[1] = v1;
	verts[2] = v2;
	cols[0] = V3(0.5f, 0.5f, 0.5f);
	cols[1] = cols[0];
	cols[2] = cols[0];
	V3 n = ((v1 - v0) ^ (v2 - v0)).UnitVector();
	normals[0] =
		normals[1] =
		normals[2] = n;

	tris = new unsigned int[3 * trisN];
	tris[0] = 0;
	tris[1] = 1;
	tris[2] = 2;
}



void TM::AdjustTexturedRectangle(float textureOffsetX, float textureOffsetY, float normalizedTextureWidth, float normalizedTextureHeight) {
	V3 startingPoint(textureOffsetX, textureOffsetY, 0.0f);
	// textureCoordinates[0] = startingPoint;
	// textureCoordinates[1] = startingPoint + V3(0.0f, normalizedTextureHeight, 0.0f);
	// textureCoordinates[2] = startingPoint + V3(normalizedTextureWidth, normalizedTextureHeight, 0.0f);
	// textureCoordinates[3] = startingPoint + V3(normalizedTextureWidth, 0.0f, 0.0f);

	// the texture is upside down, so what if i mess with the coordinates
	
	textureCoordinates[1] = startingPoint;
	textureCoordinates[0] = startingPoint + V3(0.0f, normalizedTextureHeight, 0.0f);
	textureCoordinates[3] = startingPoint + V3(normalizedTextureWidth, normalizedTextureHeight, 0.0f);
	textureCoordinates[2] = startingPoint + V3(normalizedTextureWidth, 0.0f, 0.0f);
}




// vertical rectangle in x0y plane, with O as center
// creates a rectangle centered at 0, 0
void TM::SetTexturedRectangle(float rectw, float recth, float textureOffsetX, float textureOffsetY, float normalizedTextureWidth, float normalizedTextureHeight) {

	vertsN = 4;
	verts = new V3[vertsN];
	cols = new V3[vertsN];
	normals = new V3[vertsN];
	textureCoordinates = new V3[vertsN];
	// tcs = new V3[vertsN];

//0(0, 0)       3 (1, 0)
//	   \
//		   \
//1(0, 1)       2 (1, 1)

	verts[0] = V3(-rectw / 2.0f, recth / 2.0f, 0.0f); // top left
	verts[1] = V3(-rectw / 2.0f, -recth / 2.0f, 0.0f); // bottom left
	verts[2] = V3(rectw / 2.0f, -recth / 2.0f, 0.0f); // bottom right
	verts[3] = V3(rectw / 2.0f, recth / 2.0f, 0.0f); // top right

	trisN = 2;
	tris = new unsigned int[3 * trisN];
	tris[0] = 0;
	tris[1] = 1;
	tris[2] = 2;
	tris[3] = 2;
	tris[4] = 3;
	tris[5] = 0;

	normals[0] =
		normals[1] =
		normals[2] =
		normals[3] = V3(0.0f, 0.0f, 1.0f);

	// cols[0] = cols[1] = cols[2] = cols[3] = V3(0.2f, 0.2f, 0.2f);
	cols[0] = cols[1] = cols[2] = cols[3] = V3(1, 1, 1);
	// cols[3] = V3(0.0f, 1.0f, 0.0f);

	V3 startingPoint(textureOffsetX, textureOffsetY, 0.0f);

	textureCoordinates[0] = startingPoint;
	textureCoordinates[1] = startingPoint + V3(0.0f, normalizedTextureHeight, 0.0f);
	textureCoordinates[2] = startingPoint + V3(normalizedTextureWidth, normalizedTextureHeight, 0.0f);
	textureCoordinates[3] = startingPoint + V3(normalizedTextureWidth, 0.0f, 0.0f);
}






// vertical rectangle in x0y plane, with O as center
// creates a rectangle centered at 0, 0
void TM::SetTexturedRectangle(float rectw, float recth) {

	vertsN = 4;
	verts = new V3[vertsN];
	cols = new V3[vertsN];
	normals = new V3[vertsN];
	textureCoordinates = new V3[vertsN];
	// tcs = new V3[vertsN];

//0(0, 0)       3 (1, 0)
//	   \
//		   \
//1(0, 1)       2 (1, 1)

	verts[0] = V3(-rectw / 2.0f, recth / 2.0f, 0.0f);
	verts[1] = V3(-rectw / 2.0f, -recth / 2.0f, 0.0f);
	verts[2] = V3(rectw / 2.0f, -recth / 2.0f, 0.0f);
	verts[3] = V3(rectw / 2.0f, recth / 2.0f, 0.0f);

	trisN = 2;
	tris = new unsigned int[3 * trisN];
	tris[0] = 0;
	tris[1] = 1;
	tris[2] = 2;
	tris[3] = 2;
	tris[4] = 3;
	tris[5] = 0;

	normals[0] =
		normals[1] =
		normals[2] =
		normals[3] = V3(0.0f, 0.0f, 1.0f);

	// cols[0] = cols[1] = cols[2] = cols[3] = V3(0.2f, 0.2f, 0.2f);
	cols[0] = cols[1] = cols[2] = cols[3] = V3(1.0f, 1.0f, 1.0f);
	// cols[3] = V3(0.0f, 1.0f, 0.0f);

	// float tc = 10.0f;
	float tc = 1.0f;
	textureCoordinates[0] = V3(0.0f, 0.0f, 0.0f);
	textureCoordinates[1] = V3(0.0f, tc, 0.0f);
	textureCoordinates[2] = V3(tc, tc, 0.0f);
	textureCoordinates[3] = V3(tc, 0.0f, 0.0f);
}



// hardware stuff
void TM::RenderToHardware() {

	// i think this is from the specify geometry slide

	if (texture) {

		// GLuint texnum;
		// textureNum is a field now
		int texwidth = texture->w;
		int texheight = texture->h;

		glEnable(GL_TEXTURE_2D);
        // glGenTextures(1, &texnum);
        // glBindTexture(GL_TEXTURE_2D, texnum);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindTexture(GL_TEXTURE_2D, textureNum); // set the active texture

        glTexCoordPointer(3, GL_FLOAT, 0, textureCoordinates); 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->pix);

        // glVertexPointer(3, GL_FLOAT, 0, verts); // already calling this below

		
		// new: now i can call this function to set the active texture
		// glActiveTexture(textureNum); // <-- i think this can be substituded for textureNums[0] aka textureNum



		/* old code that works
		// happens in init
		GLuint texnum;
		int texwidth = texture->w;
		int texheight = texture->h;

		glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texnum);
        glBindTexture(GL_TEXTURE_2D, texnum);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glTexCoordPointer(3, GL_FLOAT, 0, textureCoordinates); 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedtexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->pix);

        glVertexPointer(3, GL_FLOAT, 0, verts);
		*/	


	}

	// old
	/*
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, verts);
	glColorPointer(3, GL_FLOAT, 0, cols);
	glDrawElements(GL_TRIANGLES, 3 * trisN, GL_UNSIGNED_INT, tris);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	*/

	// new
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, verts);
	glColorPointer(3, GL_FLOAT, 0, cols);
	// if (normals)
	glNormalPointer(GL_FLOAT, 0, normals);
	glDrawElements(GL_TRIANGLES, 3 * trisN, GL_UNSIGNED_INT, tris);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	//

	if (texture) {
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

}

// create an AABB using this TM
AABB TM::SetAABB() {
	AABB ret(verts[0]);
	for (int vi = 0; vi < vertsN; vi++)
		ret.AddPoint(verts[vi]);
	return ret;
}