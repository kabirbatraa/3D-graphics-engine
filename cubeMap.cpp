#include "cubeMap.h"

#include <tiffio.h>
#include <iostream>


CubeMap::CubeMap(char* tiffFileName) {

  // cout << "constructor" << endl;

  // load the tiff 
  LoadTiff(tiffFileName);
  subW = w / 3;
  subH = h / 4;

  // create the six cameras

  V3 center(0.0f, 0.0f, 0.0f);

  // PPC *topCamera = cameraPointers[0];
  // PPC *bottomCamera = cameraPointers[1];
  // PPC *leftCamera = cameraPointers[2];
  // PPC *rightCamera = cameraPointers[3];
  // PPC *frontCamera = cameraPointers[4];
  // PPC *backCamera = cameraPointers[5];

  // fov should be 90 degrees for each camera
  topCamera = new PPC(90.0f, subW, subH);
  bottomCamera = new PPC(90.0f, subW, subH);
  leftCamera = new PPC(90.0f, subW, subH);
  rightCamera = new PPC(90.0f, subW, subH);
  frontCamera = new PPC(90.0f, subW, subH);
  backCamera = new PPC(90.0f, subW, subH);

  // cameraPointers = {
  //   topCamera, 
  //   bottomCamera,
  //   leftCamera,
  //   rightCamera,
  //   frontCamera,
  //   backCamera,
  // };
  cameraPointers[0] = topCamera;
  cameraPointers[1] = bottomCamera;
  cameraPointers[2] = leftCamera;
  cameraPointers[3] = rightCamera;
  cameraPointers[4] = frontCamera;
  cameraPointers[5] = backCamera;

  topCamera->PositionAndOrient(center, V3(0.0f, 1.0f, 0.0f), V3(0.0f, 0.0f, 1.0f)); // positive y and forward (positive z)
  bottomCamera->PositionAndOrient(center, V3(0.0f, -1.0f, 0.0f), V3(0.0f, 0.0f, -1.0f)); // negative y and backward
  leftCamera->PositionAndOrient(center, V3(-1.0f, 0.0f, 0.0f), V3(0.0f, 1.0f, 0.0f)); // negative x and up
  rightCamera->PositionAndOrient(center, V3(1.0f, 0.0f, 0.0f), V3(0.0f, 1.0f, 0.0f)); // positive x and up
  frontCamera->PositionAndOrient(center, V3(0.0f, 0.0f, 1.0f), V3(0.0f, 1.0f, 0.0f)); // positive z and up
  backCamera->PositionAndOrient(center, V3(0.0f, 0.0f, -1.0f), V3(0.0f, -1.0f, 0.0f)); // negative z and down

  previousCamera = 4; // front

  // cout << "constructor end" << endl;

  // cout << topCamera->a << endl;
  // cout << topCamera->b << endl;

}



// load a tiff image to pixel buffer
void CubeMap::LoadTiff(char* tiffFileName) {
	TIFF* in = TIFFOpen(tiffFileName, "r");
	if (in == NULL) {
		cerr << tiffFileName << " could not be opened" << endl;
		return;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		w = width;
		h = height;
    
		delete[] mainPixelArray;
		mainPixelArray = new unsigned int[w * h];
	}

  // loading the tiff requires a pix array it seems? well if its a 2d array it will still work.. right?
	if (TIFFReadRGBAImage(in, w, h, mainPixelArray, 0) == 0) {
		cerr << "failed to load " << tiffFileName << endl;
	}

	TIFFClose(in);
}




unsigned int CubeMap::GetColor(V3 direction) {

  PPC* previous = cameraPointers[previousCamera];
  V3 projectedPoint;
  previous->Project(direction, projectedPoint);

  if (
    projectedPoint[2] > 0
    && 0 <= projectedPoint[0] && projectedPoint[0] < subW
    && 0 <= projectedPoint[1] && projectedPoint[1] < subH
  ) {
    return GetBilinearlyInterpolatedColor(projectedPoint[0], projectedPoint[1], previousCamera);
    // return GetColorFromRespectivePixelArray((int)projectedPoint[0], (int)projectedPoint[1], previousCamera);
  }



  for (int i = 0; i < 6; i++) {
    PPC* currentCamera = cameraPointers[i];
    V3 projectedPoint;
    currentCamera->Project(direction, projectedPoint);

    if (
      projectedPoint[2] > 0
      && 0 <= projectedPoint[0] && projectedPoint[0] < subW
      && 0 <= projectedPoint[1] && projectedPoint[1] < subH
    ) {
      // point is on the framebuffer

      previousCamera = i;
      return GetBilinearlyInterpolatedColor(projectedPoint[0], projectedPoint[1], previousCamera);
      // return GetColorFromRespectivePixelArray((int)projectedPoint[0], (int)projectedPoint[1], i);
    }
    else {continue;}
  }

  // cout << "what" << direction << endl;

  return 0;
}

unsigned int CubeMap::GetColorFromRespectivePixelArray(int u, int v, int cameraIndex) {
  int xOffset = 0;
  int yOffset = 0;

  // PPC *topCamera;
  if (cameraIndex == 0) {
    xOffset = subW;
    yOffset = 0;
  }
  // PPC *bottomCamera;
  else if (cameraIndex == 1) {
    xOffset = subW;
    yOffset = 2*subH;
  }
  // PPC *leftCamera;
  else if (cameraIndex == 2) {
    xOffset = 0;
    yOffset = subH;
  }
  // PPC *rightCamera;
  else if (cameraIndex == 3) {
    xOffset = 2*subW;
    yOffset = subH;
  }
  // PPC *frontCamera;
  else if (cameraIndex == 4) {
    xOffset = subW;
    yOffset = subH;
  }
  // PPC *backCamera;
  else if (cameraIndex == 5) {
    xOffset = subW;
    yOffset = 3*subH;
  }

  u += xOffset;
  v += yOffset;

  // u is x is column
  // v is y is row
  if (0 <= u && u < w && 0 <= v && v < h) {
    return mainPixelArray[(h - 1 - v)*w + u]; // copied from get from framebuffer, because if loadBin works, then it must follow this format
  }
  else {
    cout << "failed to get color from mainPixelArray" << endl;
    return 0xFFFFFF00;
  }
  // honestly have no idea why we dont just do this:
  // return mainPixelArray[u + v*w]
}




unsigned int CubeMap::GetBilinearlyInterpolatedColor(float uf, float vf, int cameraIndex) {
  // if we are working with a pixel that is on the edge, then just get instead of bilinear
	if (uf < 0.5f || vf < 0.5f || uf > -.5f + (float)subW || vf > -.5f + (float)subH) {
    int u = (int)uf;
		int v = (int)vf;
		return GetColorFromRespectivePixelArray(u, v, cameraIndex);
	}

	int uA = (int)(uf - 0.5f);
	int vA = (int)(vf - 0.5f);
	float x = uf - (.5f + (float)uA);
	float y = vf - (.5f + (float)vA);
	
	V3 A; A.SetFromColor(GetColorFromRespectivePixelArray(uA, vA, cameraIndex));
	V3 B; B.SetFromColor(GetColorFromRespectivePixelArray(uA, vA+1, cameraIndex));
	V3 C; C.SetFromColor(GetColorFromRespectivePixelArray(uA+1, vA+1, cameraIndex));
	V3 D; D.SetFromColor(GetColorFromRespectivePixelArray(uA+1, vA, cameraIndex));
	V3 bic = A*(1 - x)*(1 - y) + B*(1 - x)*y + C*x*y + D*x*(1 - y);
	
	return bic.GetColor();

}

