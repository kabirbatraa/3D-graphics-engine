#pragma once

#include "V3.h"

class ShadowMap {
public: 
  int w;
  int h;
  float *zb;

  ShadowMap(int w, int h);
  void SetZB(float zf);
  int IsCloserThenSet(float currz, int u, int v);
  bool FastPointInTriangle(V3 A, V3 B, V3 C, V3 P);
  void AddTriangle(V3 p0, V3 p1, V3 p2);

  float GetZB(int u, int v);

};