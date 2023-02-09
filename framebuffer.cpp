
#include "framebuffer.h"
#include "math.h"
#include <iostream>
#include "scene.h"
#include "ppc.h"
#include "M33.h"
#include "cubeMap.h"

#include <tiffio.h>

using namespace std;

FrameBuffer::FrameBuffer() : Fl_Gl_Window(0,0,0,0,0) {
	// FrameBuffer(0,0,0,0);
	w = 0;
	h = 0;
	pix = 0;
	zb = 0;
	isHardwareFB = false;
}

FrameBuffer::FrameBuffer(int u0, int v0, int _w, int _h) : 
	Fl_Gl_Window(u0, v0, _w, _h, 0) {

	isHardwareFB = false;

	w = _w;
	h = _h;
	pix = new unsigned int[w*h];
	zb = new float[w * h];

}


void FrameBuffer::draw() {

	// draw the pixels using the computed pixels in the pixels array 
	if (!isHardwareFB) {
		glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);
	}

	// render using hardware
	else {
		scene->RenderHardware();
		glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);
	}


}

int FrameBuffer::handle(int event) {

	switch (event)
	{
	case FL_KEYBOARD: {
		KeyboardHandle();
		return 0;
	}
	case FL_MOVE: {
		int u = Fl::event_x();
		int v = Fl::event_y();
		if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
			return 0;
		// cerr << u << " " << v << "      \r";
		V3 cv; cv.SetFromColor(pix[(h - 1 - v)*w + u]);
		// cerr << u << " " << v << " " << cv << "          \r";
		// cerr << u << " " << v << " " << cv[0] * 2 - 1 << " " << cv[1] * 2 - 1 << " " << cv[2] * 2 - 1 << "          \r";
		cerr << u << " " << v << " " << cv[0] * 200 - 100 << " " << cv[1] * 200 - 100 << " " << cv[2] * 200 - 100 << "          \r";
		// cerr << u << " " << v << " " << (cv[0] * 1000) - 500 << "          \r";
		return 0;
	}
	default:
		
		return 0;
	}
	return 0;
}

void FrameBuffer::KeyboardHandle() {
	int key = Fl::event_key();
	// cout << (char)key << endl;
	if (key == 'w') {
		scene->Forward();
	}
	else if (key == 's') {
		scene->Backward();
	}
	else if (key == 'a') {
		scene->Leftward();
	}
	else if (key == 'd') {
		scene->Rightward();
	}
	else if (key == FL_Up) {
		scene->LookUp();
	}
	else if (key == FL_Down) {
		scene->LookDown();
	}
	else if (key == FL_Right) {
		scene->LookRight();
	}
	else if (key == FL_Left) {
		scene->LookLeft();
	}
}

void FrameBuffer::SetBGR(unsigned int bgr) {

	for (int uv = 0; uv < w*h; uv++)
		pix[uv] = bgr;

}

void FrameBuffer::SetZB(float zf) {

	for (int uv = 0; uv < w*h; uv++)
		zb[uv] = zf;

}

void FrameBuffer::SetChecker(unsigned int col0, unsigned int col1, int csize) {

	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			int cu = u / csize;
			int cv = v / csize;
			if ((cu + cv) % 2)
				Set(u, v, col0);
			else
				Set(u, v, col1);
		}
	}

}

void FrameBuffer::DrawAARectangle(V3 tlc, V3 brc, unsigned int col) {

	// entire rectangle off screen
	if (tlc[0] > (float)w)
		return;
	if (brc[0] < 0.0f)
		return;
	if (tlc[1] > (float)h)
		return;
	if (brc[1] < 0.0f)
		return;

	// rectangle partially off screen
	if (tlc[0] < 0.0f)
		tlc[0] = 0.0f;
	if (brc[0] > (float)w)
		brc[0] = (float)w;
	if (tlc[1] < 0.0f)
		tlc[1] = 0.0f;
	if (brc[1] > (float)h)
		brc[1] = (float)h;

	int umin = (int)(tlc[0]+0.5f);
	int umax = (int)(brc[0]-0.5f);
	int vmin = (int)(tlc[1]+0.5f);
	int vmax = (int)(brc[1]-0.5f);
	for (int v = vmin; v <= vmax; v++) {
		for (int u = umin; u <= umax; u++) {
			// Set(u, v, col);
			SetGuarded(u, v, col);
		}
	}

}

void FrameBuffer::DrawDisk(V3 center, float r, unsigned int col) {

	int umin = (int)(center[0] - r);
	int umax = (int)(center[0] + r);
	int vmin = (int)(center[1] - r);
	int vmax = (int)(center[1] + r);
	center[2] = 0.0f;
	for (int v = vmin; v <= vmax; v++) {
		for (int u = umin; u <= umax; u++) {
			V3 pixCenter(.5f + (float)u, .5f + (float)v, 0.0f);
			V3 distVector = pixCenter - center;
			if (r*r < distVector * distVector)
				continue;
			// Set(u, v, col);
			SetGuarded(u, v, col);
		}
	}


}


void FrameBuffer::Set(int u, int v, int col) {

	pix[(h - 1 - v)*w + u] = col;

}

// set but inverted! used for getting the pixel color assuming this framebuffer is a textureMap
unsigned int FrameBuffer::Get(int u, int v) {
	return pix[(h - 1 - v)*w + u];
}


void FrameBuffer::SetGuarded(int u, int v, int col) {

	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return;
	Set(u, v, col);

}

void FrameBuffer::DrawSegment(V3 p0, V3 p1, unsigned int col) {

	V3 cv;
	cv.SetFromColor(col);
	DrawSegment(p0, p1, cv, cv);

}

void FrameBuffer::DrawSegment(V3 p0, V3 p1, V3 c0, V3 c1) {

	float maxSpan = (fabsf(p0[0] - p1[0]) < fabsf(p0[1] - p1[1])) ?
		fabsf(p0[1] - p1[1]) : fabsf(p0[0] - p1[0]);
	int segsN = (int)maxSpan + 1;

	if (segsN > 1000)
		return; // super long segments cause lag
		// cout << segsN << endl;

	V3 currPoint = p0;
	V3 currColor = c0;
	V3 stepv = (p1 - p0) / (float)segsN;
	V3 stepcv = (c1 - c0) / (float)segsN;
	int si;
	for (si = 0; 
		si <= segsN; 
		si++, currPoint = currPoint + stepv, currColor = currColor + stepcv) {
		int u = (int)currPoint[0];
		int v = (int)currPoint[1];
		if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
			continue;
		if (IsCloserThenSet(currPoint[2], u, v))
			SetGuarded(u, v, currColor.GetColor());
	}

}


// load a tiff image to pixel buffer
void FrameBuffer::LoadTiff(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		if (pix) {
			delete[] pix;
		}
		w = width;
		h = height;
		
		pix = new unsigned int[w*h];
		size(w, h);
		glFlush();
		glFlush();
	}

	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}

	TIFFClose(in);
}

// save as tiff image
void FrameBuffer::SaveAsTiff(char *fname) {

	TIFF* out = TIFFOpen(fname, "w");

	if (out == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (uint32 row = 0; row < (unsigned int)h; row++) {
		TIFFWriteScanline(out, &pix[(h - row - 1) * w], row);
	}

	TIFFClose(out);
}


void FrameBuffer::Render3DSegment(PPC *ppc, V3 v0, V3 v1, V3 c0, V3 c1) {
	V3 pv0, pv1;
	if (!ppc->Project(v0, pv0))
		return;
	if (!ppc->Project(v1, pv1))
		return;
	DrawSegment(pv0, pv1, c0, c1);
}

void FrameBuffer::Render3DPoint(PPC *ppc, V3 p, V3 c, float psize) {
	V3 pv;
	if (!ppc->Project(p, pv))
		return;
	DrawDisk(pv, psize, c.GetColor());
}

// takes in 1 / w for the z value
// and is also setting 1 / w in the z depth array
int FrameBuffer::IsCloserThenSet(float currz, int u, int v) {

	// lets make this guarded:
	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return 0;
	
	float zbz = zb[(h - 1 - v)*w + u];
	if (zbz > currz)
		return 0;
	zb[(h - 1 - v)*w + u] = currz;
	return 1;

}





unsigned int FrameBuffer::CreateColor(int r, int g, int b) {
	
	unsigned int color = 0xFF0000000;
	color += r;
	color += g << 8;
	color += b << 16;
	
	return color;
}


void FrameBuffer::DrawTriangle(V3 p0, V3 p1, V3 p2, unsigned int col) {

	int up = min(min(p0[1], p1[1]), p2[1]);
	int down = max(max(p0[1], p1[1]), p2[1]);
	int left = min(min(p0[0], p1[0]), p2[0]);
	int right = max(max(p0[0], p1[0]), p2[0]);

	for (int i = left; i <= right; i++) {
		for (int j = up; j <= down; j++) {
			V3 currentPoint(i, j, 0);
			// if (point in triangle)
			// bool sidedness1 = Sidedness(p0, p1, p2) == Sidedness(p0, p1, currentPoint);
			// bool sidedness2 = Sidedness(p1, p2, p0) == Sidedness(p1, p2, currentPoint);
			// bool sidedness3 = Sidedness(p0, p2, p1) == Sidedness(p0, p2, currentPoint);
			// if (sidedness1 && sidedness2 && sidedness3) {
			if (FastPointInTriangle(p0, p1, p2, currentPoint)) {
				SetGuarded(i, j, col);
			}
		}
	}
}

bool FrameBuffer::Sidedness(V3 edgePoint1, V3 edgePoint2, V3 point) {
	int x1, x2, x3, y1, y2, y3;
	x1 = edgePoint1[0];
	x2 = edgePoint2[0];
	x3 = point[0];
	y1 = edgePoint1[1];
	y2 = edgePoint2[1];
	y3 = point[1];

	return 0 < x3 * (y2 - y1) - y3 * (x2 - x1) - x1 * y2 + y1 * x2;
}

void FrameBuffer::DrawTriangleInterpolatedColor(V3 p0, V3 p1, V3 p2, V3 c0, V3 c1, V3 c2) {
	int up = min(min(p0[1], p1[1]), p2[1]);
	int down = max(max(p0[1], p1[1]), p2[1]);
	int left = min(min(p0[0], p1[0]), p2[0]);
	int right = max(max(p0[0], p1[0]), p2[0]);

	// if (down - up < 5) return;
	// if (right - left < 5) return;

	for (int i = left; i <= right; i++) {
		for (int j = up; j <= down; j++) {
			V3 currentPoint(i, j, 0);
			// if (point in triangle)
			// bool sidedness1 = Sidedness(p0, p1, p2) == Sidedness(p0, p1, currentPoint);
			// bool sidedness2 = Sidedness(p1, p2, p0) == Sidedness(p1, p2, currentPoint);
			// bool sidedness3 = Sidedness(p0, p2, p1) == Sidedness(p0, p2, currentPoint);
			// if (sidedness1 && sidedness2 && sidedness3) {
			if (FastPointInTriangle(p0, p1, p2, currentPoint)) {
			

				// screenspace interpolation to get pixel's color here

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

				V3 rValues(c0[0], c1[0], c2[0]);
				V3 gValues(c0[1], c1[1], c2[1]);
				V3 bValues(c0[2], c1[2], c2[2]);
				// let ood mean one over depth
				V3 oodValues(p0[2], p1[2], p2[2]);

				V3 abcForR = invertedLeftMatrix * rValues;
				V3 abcForG = invertedLeftMatrix * gValues;
				V3 abcForB = invertedLeftMatrix * bValues;
				V3 abcForOod = invertedLeftMatrix * oodValues;

				float rAtPoint = abcForR[0] * u + abcForR[1] * v + abcForR[2];
				float gAtPoint = abcForG[0] * u + abcForG[1] * v + abcForG[2];
				float bAtPoint = abcForB[0] * u + abcForB[1] * v + abcForB[2];
				float oodAtPoint = abcForOod[0] * u + abcForOod[1] * v + abcForOod[2];

				V3 finalColor(rAtPoint, gAtPoint, bAtPoint);

				if (IsCloserThenSet(oodAtPoint, u, v)) {

					SetGuarded(i, j, finalColor.GetColor());
				}
			}
		}
	}
}



void FrameBuffer::DrawTriangleWithLightAndNormals(V3 p0, V3 p1, V3 p2, PPC *ppc, V3 matColor, float ka, V3 lightPosition, V3 normal0, V3 normal1, V3 normal2) {
	int up = min(min(p0[1], p1[1]), p2[1]);
	int down = max(max(p0[1], p1[1]), p2[1]);
	int left = min(min(p0[0], p1[0]), p2[0]);
	int right = max(max(p0[0], p1[0]), p2[0]);

	// if (down - up < 5) return;
	// if (right - left < 5) return;

	for (int i = left; i <= right; i++) {
		for (int j = up; j <= down; j++) {
			V3 currentPoint(i, j, 0);
			// if (point in triangle)
			// bool sidedness1 = Sidedness(p0, p1, p2) == Sidedness(p0, p1, currentPoint);
			// bool sidedness2 = Sidedness(p1, p2, p0) == Sidedness(p1, p2, currentPoint);
			// bool sidedness3 = Sidedness(p0, p2, p1) == Sidedness(p0, p2, currentPoint);
			// if (sidedness1 && sidedness2 && sidedness3) {
			if (FastPointInTriangle(p0, p1, p2, currentPoint)) {

				// screenspace interpolation to get pixel's color here

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

				// unproject this current point to get the direction of the light
				V3 currentPoint(u, v, oodAtPoint);
				V3 unprojectedPoint;
				ppc->Unproject(currentPoint, unprojectedPoint);

				// light direction
				V3 ld = (unprojectedPoint - lightPosition).UnitVector();
				V3 colorWithLighting = unprojectedPoint.Light(matColor, ka, ld, finalNormal);

				if (IsCloserThenSet(oodAtPoint, u, v)) {

					SetGuarded(i, j, colorWithLighting.GetColor());
				}
			}
		}
	}
}




void FrameBuffer::DrawTriangleWithLightAndNormalsAndSpecular(V3 p0, V3 p1, V3 p2, PPC *ppc, V3 matColor, float ka, V3 lightPosition, V3 normal0, V3 normal1, V3 normal2, float es) {
	int up = min(min(p0[1], p1[1]), p2[1]);
	int down = max(max(p0[1], p1[1]), p2[1]);
	int left = min(min(p0[0], p1[0]), p2[0]);
	int right = max(max(p0[0], p1[0]), p2[0]);

	// if (down - up < 5) return;
	// if (right - left < 5) return;

	for (int i = left; i <= right; i++) {
		for (int j = up; j <= down; j++) {
			V3 currentPoint(i, j, 0);
			// if (point in triangle)
			// bool sidedness1 = Sidedness(p0, p1, p2) == Sidedness(p0, p1, currentPoint);
			// bool sidedness2 = Sidedness(p1, p2, p0) == Sidedness(p1, p2, currentPoint);
			// bool sidedness3 = Sidedness(p0, p2, p1) == Sidedness(p0, p2, currentPoint);
			// if (sidedness1 && sidedness2 && sidedness3) {
			if (FastPointInTriangle(p0, p1, p2, currentPoint)) {
				// screenspace interpolation to get pixel's color here

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

				// unproject this current point to get the direction of the light
				V3 currentPoint(u, v, oodAtPoint);
				V3 unprojectedPoint;
				ppc->Unproject(currentPoint, unprojectedPoint);
				V3 thisPointPosition = unprojectedPoint;

				// light direction
				V3 ld = (unprojectedPoint - lightPosition).UnitVector();
				V3 eyePosition = ppc->C;
				V3 colorWithLightingAndSpecular = unprojectedPoint.LightWithSpecular(matColor, ka, ld, finalNormal, es, eyePosition, lightPosition, thisPointPosition);


				if (IsCloserThenSet(oodAtPoint, u, v)) {

					SetGuarded(i, j, colorWithLightingAndSpecular.GetColor());
					// SetGuarded(i, j, 0x00000000);
				}
			}
		}
	}

}







void FrameBuffer::DrawTriangleWithColorsAndLightAndNormalsAndSpecular(V3 p0, V3 p1, V3 p2, PPC *ppc, V3 c0, V3 c1, V3 c2, float ka, V3 lightPosition, V3 normal0, V3 normal1, V3 normal2, float es) {
	int up = min(min(p0[1], p1[1]), p2[1]);
	int down = max(max(p0[1], p1[1]), p2[1]);
	int left = min(min(p0[0], p1[0]), p2[0]);
	int right = max(max(p0[0], p1[0]), p2[0]);

	// if (down - up < 5) return;
	// if (right - left < 5) return;

	for (int i = left; i <= right; i++) {
		for (int j = up; j <= down; j++) {
			V3 currentPoint(i, j, 0);
			// if (point in triangle)
			// bool sidedness1 = Sidedness(p0, p1, p2) == Sidedness(p0, p1, currentPoint);
			// bool sidedness2 = Sidedness(p1, p2, p0) == Sidedness(p1, p2, currentPoint);
			// bool sidedness3 = Sidedness(p0, p2, p1) == Sidedness(p0, p2, currentPoint);
			// if (sidedness1 && sidedness2 && sidedness3) {
			if (FastPointInTriangle(p0, p1, p2, currentPoint)) {
				// screenspace interpolation to get pixel's color here

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

				// unproject this current point to get the direction of the light
				V3 currentPoint(u, v, oodAtPoint);
				V3 unprojectedPoint;
				ppc->Unproject(currentPoint, unprojectedPoint);
				V3 thisPointPosition = unprojectedPoint;

				// light direction
				V3 ld = (unprojectedPoint - lightPosition).UnitVector();
				V3 eyePosition = ppc->C;
				V3 colorWithLightingAndSpecular = unprojectedPoint.LightWithSpecular(matColor, ka, ld, finalNormal, es, eyePosition, lightPosition, thisPointPosition);

				if (IsCloserThenSet(oodAtPoint, u, v)) {

					SetGuarded(i, j, colorWithLightingAndSpecular.GetColor());
					// SetGuarded(i, j, 0x00000000);
				}
			}
		}
	}

}





void FrameBuffer::DrawTriangleWithColorsAndLightAndNormalsAndSpecularAndShadowMap(V3 p0, V3 p1, V3 p2, PPC *ppc, V3 c0, V3 c1, V3 c2, PPC *lightPPC, ShadowMap *sm, float ka, V3 lightPosition, V3 normal0, V3 normal1, V3 normal2, float es) {
	int up = min(min(p0[1], p1[1]), p2[1]);
	int down = max(max(p0[1], p1[1]), p2[1]);
	int left = min(min(p0[0], p1[0]), p2[0]);
	int right = max(max(p0[0], p1[0]), p2[0]);

	// if (down - up < 5) return;
	// if (right - left < 5) return;

	for (int i = left; i <= right; i++) {
		for (int j = up; j <= down; j++) {
			V3 currentPoint(i, j, 0);
			// if (point in triangle)
			// bool sidedness1 = Sidedness(p0, p1, p2) == Sidedness(p0, p1, currentPoint);
			// bool sidedness2 = Sidedness(p1, p2, p0) == Sidedness(p1, p2, currentPoint);
			// bool sidedness3 = Sidedness(p0, p2, p1) == Sidedness(p0, p2, currentPoint);
			// if (sidedness1 && sidedness2 && sidedness3) {
			if (FastPointInTriangle(p0, p1, p2, currentPoint)) {
				// screenspace interpolation to get pixel's color here

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

				if (IsCloserThenSet(oodAtPoint, u, v)) {

					SetGuarded(i, j, colorToUse.GetColor());
					// SetGuarded(i, j, 0x00000000);
				}
			}
		}
	}

}





bool FrameBuffer::FastPointInTriangle(V3 A, V3 B, V3 C, V3 P) {

	float s1 = C[1] - A[1];
	float s2 = C[0] - A[0];
	float s3 = B[1] - A[1];
	float s4 = P[1] - A[1];

	float w1 = (A[0] * s1 + s4 * s2 - P[0] * s1) / (s3 * s2 - (B[0]-A[0]) * s1);
	float w2 = (s4- w1 * s3) / s1;
	return w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1;
}


// returns color of texture at the normalized coordinate values
// if greater than 1, then mod 1 is taken first 
unsigned int FrameBuffer::GetTexelColor(float normalizedS, float normalizedT) {

  // return error color for negative coordinates
	if (normalizedS < 0.0f || normalizedT < 0.0f)
		return 0xFFFFFF00;
	
	// if value greater than 1, then take mod 1
	// normalizedS = normalizedS % 1;
	// normalizedT = normalizedT % 1;
	// mod doesnt work with float so alternative: 
	if (mirrorTiling) {
		
		// for S
		float mod2 = normalizedS - (float)((int)(normalizedS / 2.0f)) * 2.0f;
		if (mod2 < 1) {
			normalizedS = mod2;
		}
		else {
			normalizedS = 1 - (mod2 - 1);
		}

		// for T
		mod2 = normalizedT - (float)((int)(normalizedT / 2.0f)) * 2.0f;
		if (mod2 < 1) {
			normalizedT = mod2;
		}
		else {
			normalizedT = 1 - (mod2 - 1);
		}

	}
	else {
		// takes just the decimal part of the number
		normalizedS = normalizedS - (float)((int)normalizedS);
		normalizedT = normalizedT - (float)((int)normalizedT);
	}

	// if (normalizedS >= 1 || normalizedT >= 1) {
	// 	cout << "error" << normalizedS << " " << normalizedT << endl;
	// }

	int u = (int)(normalizedS * (float)w - 0.5f);
	int v = (int)(normalizedT * (float)h - 0.5f);
	return Get(u, v);

}


// uses bilinear interpolation to get texel color
unsigned int FrameBuffer::GetTexelColorBilinear(float normalizedS, float normalizedT) {
	if (normalizedS < 0.0f || normalizedT < 0.0f)
		return 0xFFFFFF00;
	if (mirrorTiling) {
		float mod2 = normalizedS - (float)((int)(normalizedS / 2.0f)) * 2.0f;
		if (mod2 < 1) {
			normalizedS = mod2;
		}
		else {
			normalizedS = 1 - (mod2 - 1);
		}
		mod2 = normalizedT - (float)((int)(normalizedT / 2.0f)) * 2.0f;
		if (mod2 < 1) {
			normalizedT = mod2;
		}
		else {
			normalizedT = 1 - (mod2 - 1);
		}
	}
	else {
		normalizedS = normalizedS - (float)((int)normalizedS);
		normalizedT = normalizedT - (float)((int)normalizedT);
	}
	

	float sf = normalizedS;
	float tf = normalizedT;

	float uf = sf * (float)w;
	float vf = tf * (float)h;
	// if we are working with a pixel that is on the edge, then just get instead of bilinear
	if (uf < 0.5f || vf < 0.5f || uf > -.5f + (float)w || vf > -.5f + (float)h) {
		// int u = (int)(normalizedS * (float)w); // what is the meaning of these..? sf * w is just uf
		// int v = (int)(normalizedT * (float)h);
		int u = (int)uf;
		int v = (int)vf;
		return Get(u, v);
	}

	int uA = (int)(uf - 0.5f);
	int vA = (int)(vf - 0.5f);
	float x = uf - (.5f + (float)uA);
	float y = vf - (.5f + (float)vA);
	
	V3 A; A.SetFromColor(Get(uA, vA));
	V3 B; B.SetFromColor(Get(uA, vA+1));
	V3 C; C.SetFromColor(Get(uA+1, vA+1));
	V3 D; D.SetFromColor(Get(uA+1, vA));
	V3 bic = A*(1 - x)*(1 - y) + B*(1 - x)*y + C*x*y + D*x*(1 - y);
	
	return bic.GetColor();

}

/*
unsigned int FrameBuffer::LookUpBilinear(float s, float t) {

	if (s < 0.0f || t < 0.0f)
		return 0xFFFFFF00;

	float sf = s - (float)((int)s);
	float tf = t - (float)((int)t);

	float uf = sf * (float)w;
	float vf = tf * (float)h;
	if (uf < 0.5f || vf < 0.5f || uf > -.5f + (float)w || vf > -.5f + (float)h)
		return LookUpNN(s, t);

	int uA = (int)(uf - 0.5f);
	int vA = (int)(vf - 0.5f);
	float x = uf - (.5f + (float)uA);
	float y = vf - (.5f + (float)vA);
	
	V3 A; A.SetFromColor(Get(uA, vA));
	V3 B; B.SetFromColor(Get(uA, vA+1));
	V3 C; C.SetFromColor(Get(uA+1, vA+1));
	V3 D; D.SetFromColor(Get(uA+1, vA));
	V3 bic = A*(1 - x)*(1 - y) + B*(1 - x)*y + C*x*y + D*x*(1 - y);
	
	return bic.GetColor();
}
*/


void FrameBuffer::RenderEnvironmentMap(CubeMap *em, PPC *ppc) {
	for (int u = 0; u < w; u++) {
		for (int v = 0; v < h; v++) {
			// get direction using u and v and camera
			V3 direction = ppc->c + ppc->a * u + ppc->b * v - ppc->C;
			unsigned int col = em->GetColor(direction);
			Set(u, v, col);
		}
	}
}
