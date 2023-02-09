#pragma once

#include "V3.h"

// axis aligned bounding box
class AABB {
public:
	V3 minv, maxv;
	AABB(V3 firstPoint);
	void AddPoint(V3 newPoint);
	int ClipWithFrame(int w, int h);
};