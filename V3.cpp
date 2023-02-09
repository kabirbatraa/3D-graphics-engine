
#include "V3.h"
#include "M33.h"

using namespace std;

#include <iostream>

V3::V3() {
	xyz[0] = 0;
	xyz[1] = 0;
	xyz[2] = 0;
}

V3::V3(float x, float y, float z) {
	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = z;
}

V3 V3::operator+(V3 v1) {
	V3 ret;
	V3 &v0 = *this;
	ret[0] = v0[0] + v1[0];
	ret[1] = v0[1] + v1[1];
	ret[2] = v0[2] + v1[2];
	return ret;
}

V3 V3::operator-(V3 v1) {
	V3 ret;
	V3 &v0 = *this;
	ret[0] = v0[0] - v1[0];
	ret[1] = v0[1] - v1[1];
	ret[2] = v0[2] - v1[2];
	return ret;
}

float& V3::operator[](int i) {
	return xyz[i];
}

float V3::operator*(V3 v1) {
	V3 &v0 = *this;
	return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

V3 V3::operator^(V3 v1) {
	V3 &v0 = *this;
	V3 ret;
	ret[0] = v0[1] * v1[2] - v0[2] * v1[1];
	ret[1] = -1 * (v0[0] * v1[2] - v0[2] * v1[0]);
	ret[2] = v0[0] * v1[1] - v0[1] * v1[0];
	return ret;
}

ostream& operator<<(ostream& ostr, V3 v) {
	return ostr << v[0] << " " << v[1] << " " << v[2];
}

istream& operator>>(istream& istr, V3 &v) {
	istr >> v[0] >> v[1] >> v[2];
	return istr;
}

V3 V3::operator*(float scalar) {
	V3 ret;
	ret[0] = this->xyz[0] * scalar;
	ret[1] = this->xyz[1] * scalar;
	ret[2] = this->xyz[2] * scalar;
	return ret;
}

V3 V3::operator*(int scalar) {
	V3 ret;
	ret[0] = this->xyz[0] * scalar;
	ret[1] = this->xyz[1] * scalar;
	ret[2] = this->xyz[2] * scalar;
	return ret;
}

V3 V3::operator/(float scalar) {
	V3 ret;
	ret[0] = this->xyz[0] / scalar;
	ret[1] = this->xyz[1] / scalar;
	ret[2] = this->xyz[2] / scalar;
	return ret;
}

V3 V3::operator/(int scalar) {
	V3 ret;
	ret[0] = this->xyz[0] / scalar;
	ret[1] = this->xyz[1] / scalar;
	ret[2] = this->xyz[2] / scalar;
	return ret;
}

V3 V3::normalized() {
	V3 &thisVector = *this;
	float thisVectorLength = this->length();
	V3 normalizedVector(thisVector[0] / thisVectorLength, thisVector[1] / thisVectorLength, thisVector[2] / thisVectorLength);
	return normalizedVector;
}

float V3::length() {
	V3 &thisVector = *this;
	return sqrt(thisVector * thisVector);
}

V3 V3::pointRotatedAboutAxis(V3 axisPoint1, V3 axisPoint2, float thetaDegrees) {

	// P
	V3 &pointP = *this;

	// local coordinate system
	V3 axisOrigin = axisPoint1; // Oa (origin)

	V3 aAxis = (axisPoint2 - axisPoint1).normalized();
	// make sure that the auxiliary axis is not lined up with a
	bool useX = aAxis * V3(1, 0, 0) < aAxis * V3(0, 1, 0);
	V3 bAxis = useX ? (V3(1, 0, 0) ^ aAxis).normalized() : (V3(0, 1, 0) ^ aAxis).normalized();
	// c has to be orthoginal to both (using cross product)
	V3 cAxis = aAxis ^ bAxis;
	M33 localAxisUnitVectors;
	localAxisUnitVectors[0] = aAxis;
	localAxisUnitVectors[1] = bAxis;
	localAxisUnitVectors[2] = cAxis;

	// Step 2: Change to the new coordinate system P -> P'
	V3 pPrime = localAxisUnitVectors * (pointP - axisOrigin);

	// Step 3: rotate about first axis (a)
	M33 rotationMatrix;
	useX ? rotationMatrix.SetRotX(thetaDegrees) : rotationMatrix.SetRotY(thetaDegrees);
	V3 pPrimePrime = rotationMatrix * pPrime;

	// Step 4: transform back to original coord system
	V3 ret = localAxisUnitVectors.Inverted() * pPrimePrime + axisOrigin;
	
	return ret;
}

V3 V3::thisVectorRotatedAboutDirection(V3 direction, float thetaDegrees) {

	V3 &thisDirection = *this;

	thisDirection.pointRotatedAboutAxis(V3(0,0,0), direction, thetaDegrees);
	
	return V3();
}




// is more of a helper function but ya
void V3::SetFromColor(unsigned int col) {

	V3 &v = *this;
	v[0] = (float)(((unsigned char*)(&col))[0]) / 255.0f;
	v[1] = (float)(((unsigned char*)(&col))[1]) / 255.0f;
	v[2] = (float)(((unsigned char*)(&col))[2]) / 255.0f;

}
unsigned int V3::GetColor() {

	V3 v = *this;
	v[0] = (v[0] < 0.0f) ? 0.0f : v[0];
	v[1] = (v[1] < 0.0f) ? 0.0f : v[1];
	v[2] = (v[2] < 0.0f) ? 0.0f : v[2];

	v[0] = (v[0] > 1.0f) ? 1.0f : v[0];
	v[1] = (v[1] > 1.0f) ? 1.0f : v[1];
	v[2] = (v[2] > 1.0f) ? 1.0f : v[2];

	unsigned int ret;
	int rgb[3];
	rgb[0] = (int)(v[0] * 255.0f);
	rgb[1] = (int)(v[1] * 255.0f);
	rgb[2] = (int)(v[2] * 255.0f);
	ret = 0xFF000000 + 256 * 256 * rgb[2] + 256 * rgb[1] + rgb[0];
	return ret;
}












float V3::Length() {

	V3 &v = *this;
	return sqrtf(v*v);

}

V3 V3::UnitVector() {

	V3 ret = *this;
	ret = ret / Length();
	return ret;
}

// aD means arbitrary Direction
V3 V3::RotateThisVector(V3 aD, float theta) {

	return RotateThisPoint(V3(0.0f, 0.0f, 0.0f), aD, theta);

}

// aO means arbitary Origin
// aD means arbitrary Direction
V3 V3::RotateThisPoint(V3 aO, V3 aD, float theta) {

	// aO means arbitary Origin
	// aD means arbitrary Direction

	M33 laxes;
	laxes[1] = aD;
	V3 aux(1.0f, 0.0f, 0.0f);
	if (fabsf(aD[0]) > fabsf(aD[1]))
		aux = V3(0.0f, 1.0f, 0.0f);
	laxes[2] = aux ^ aD;
	laxes[0] = laxes[1] ^ laxes[2];

	V3 &P = *this;
	V3 Pl = laxes*(P - aO);
	M33 roty; roty.SetRotY(theta);
	V3 Pr = roty * Pl;
	return laxes.Inverted()*Pr + aO;

}


V3 V3::Light(V3 matColor, float ka, V3 ld, V3 norm) {

	V3 ret;
	float kd = (ld*-1.0f)*norm;
	kd = (kd < 0.0f) ? 0.0f : kd;
	ret = matColor*(ka + (1.0f - ka)*kd);
	return ret;
}

V3 V3::LightWithSpecular(V3 matColor, float ka, V3 ld, V3 norm, float es, V3 eyePosition, V3 lightPosition, V3 thisPointPosition) {

	V3 n = norm;

	V3 eyeVector = (eyePosition - thisPointPosition).UnitVector();
	// let v be the light ray vector
	V3 v = (thisPointPosition - lightPosition);
	// vertical component aka vector normal
	V3 vn = n * (v * n); // note that this is going in the oposite direction of the normal
	V3 reflectedLightVector = v - vn * 2;
	reflectedLightVector = reflectedLightVector.UnitVector();

	float Ks = powf((eyeVector * reflectedLightVector), es);

	// the specular highlight is showing up on the back of the teapot..
	if (eyeVector * reflectedLightVector < 0) Ks = 0;

	V3 ret;
	float kd = (ld*-1.0f)*norm;
	kd = (kd < 0.0f) ? 0.0f : kd;
	ret = matColor*(ka + (1.0f - ka)*kd) + V3(1, 1, 1) * Ks;

	return ret;
}






// void V3::SetAsEdgeEquation(V3 p0, V3 p1, V3 p2) {

// 	// (x-x0)/(x1-x0) = (y-y0)/(y1-y0);
// 	// (x-x0)(y1-y0) = (y-y0)(x1-x0);
// 	// Ax+By+C = 0
// 	float x0 = p0[0], x1 = p1[0], y0 = p0[1], y1 = p1[1];
// 	float A, B, C;
// 	A = y1-y0;
// 	B = x0-x1;
// 	C = y0*(x1-x0) - x0*(y1-y0);
// 	V3 &v = *this;
// 	v[0] = A;
// 	v[1] = B;
// 	v[2] = C;

// 	p2[2] = 1.0f;
// 	if (v*p2 < 0)
// 		v = v * -1.0f;

// }
