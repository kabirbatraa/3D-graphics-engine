
#include "AABB.h"

AABB::AABB(V3 firstPoint) {

	minv = maxv = firstPoint;

}

void AABB::AddPoint(V3 newPoint) {

	for (int ci = 0; ci < 3; ci++) {
		if (minv[ci] > newPoint[ci])
			minv[ci] = newPoint[ci];
		if (maxv[ci] < newPoint[ci])
			maxv[ci] = newPoint[ci];
	}

}


int AABB::ClipWithFrame(int w, int h) {

	if (minv[0] > (float)w)
		return 0;
	if (minv[1] > (float)h)
		return 0;
	if (maxv[0] < 0.0f)
		return 0;
	if (maxv[1] < 0.0f)
		return 0;

	if (minv[0] < 0.0f)
		minv[0] = 0.0f;
	if (minv[1] < 0.0f)
		minv[1] = 0.0f;
	if (maxv[0] > (float) w)
		maxv[0] = (float) w;
	if (maxv[1] > (float) h)
		maxv[1] = (float) h;

	return 1;

}