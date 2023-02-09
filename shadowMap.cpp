
#include "shadowMap.h"

#include "V3.h"
#include "M33.h"

ShadowMap::ShadowMap(int _w, int _h) {
  w = _w;
  h = _h;

  zb = new float[w * h];
}


void ShadowMap::SetZB(float zf) {

	for (int uv = 0; uv < w*h; uv++)
		zb[uv] = zf;

}


float ShadowMap::GetZB(int u, int v) {
  // lets make this guarded:
	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return 0;

  float zbz = zb[(h - 1 - v)*w + u];
  return zbz;
}


// takes in 1 / w for the z value
// and is also setting 1 / w in the z depth array
int ShadowMap::IsCloserThenSet(float currz, int u, int v) {

	// lets make this guarded:
	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return 0;
	
	float zbz = zb[(h - 1 - v)*w + u];
	if (zbz > currz)
		return 0;
	zb[(h - 1 - v)*w + u] = currz;
	return 1;

}





bool ShadowMap::FastPointInTriangle(V3 A, V3 B, V3 C, V3 P) {

		float s1 = C[1] - A[1];
		float s2 = C[0] - A[0];
		float s3 = B[1] - A[1];
		float s4 = P[1] - A[1];

		float w1 = (A[0] * s1 + s4 * s2 - P[0] * s1) / (s3 * s2 - (B[0]-A[0]) * s1);
		float w2 = (s4- w1 * s3) / s1;
		return w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1;
	}


  
void ShadowMap::AddTriangle(V3 p0, V3 p1, V3 p2) {
	int up = min(min(p0[1], p1[1]), p2[1]);
	int down = max(max(p0[1], p1[1]), p2[1]);
	int left = min(min(p0[0], p1[0]), p2[0]);
	int right = max(max(p0[0], p1[0]), p2[0]);

	for (int i = left; i <= right; i++) {
		for (int j = up; j <= down; j++) {
			V3 currentPoint(i, j, 0);

			if (FastPointInTriangle(p0, p1, p2, currentPoint)) {

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

				V3 oodValues(p0[2], p1[2], p2[2]);
				V3 abcForOod = invertedLeftMatrix * oodValues;
				float oodAtPoint = abcForOod[0] * u + abcForOod[1] * v + abcForOod[2];

				if (IsCloserThenSet(oodAtPoint, u, v)) { // already adjusts the z buffer
					// SetGuarded(i, j, finalColor.GetColor());
				}
			}
		}
	}
}

