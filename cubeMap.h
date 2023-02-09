#pragma once

#include "ppc.h"
#include "V3.h"

class CubeMap {
public: 

  // each texture will be an array of pixels 
  // i will construct them using a 2d array of pixels 
  
  // order of cameras: 
  PPC *topCamera;
  PPC *bottomCamera;
  PPC *leftCamera;
  PPC *rightCamera;
  PPC *frontCamera;
  PPC *backCamera;

  PPC *cameraPointers[6];

  // PPC *cameraPointers[1];
  int previousCamera; // first camera to check in GetColor

  // unsigned int *topTexture;
  // unsigned int *bottomTexture;
  // unsigned int *leftTexture;
  // unsigned int *rightTexture;
  // unsigned int *frontTexture;
  // unsigned int *backTexture;

  // unsigned int **pixelArrays;
  unsigned int *mainPixelArray;
  
  // w and h of the full texture
  int w;
  int h;

  int subW, subH;

  // create a cubemap by passing in the tiff
  CubeMap(char* tiffFileName);

  // load a tiff into the 6 textures
  void LoadTiff(char* tiffFileName);

  // get the color from the correct texture by projecting this direction onto the cameras
  unsigned int GetColor(V3 direction);

  unsigned int GetColorFromRespectivePixelArray(int u, int v, int cameraIndex);
  unsigned int GetBilinearlyInterpolatedColor(float u, float v, int cameraIndex);



};