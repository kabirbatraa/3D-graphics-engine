
#pragma once

#include <ostream>
#include <istream>

using namespace std;

class V3 {
public:
	float xyz[3];
	V3(); // added
	V3(float x, float y, float z);
	V3 operator+(V3 v1);
	V3 operator-(V3 v1);
	float operator*(V3 v1);
	V3 operator^(V3 v1); // added
	V3 operator*(float scalar); // added
	V3 operator/(float scalar); // added
	V3 operator*(int scalar); // added
	V3 operator/(int scalar); // added
	float& operator[](int i);
	friend ostream& operator<<(ostream& ostr, V3 v);
	friend istream& operator>>(istream& istr, V3 &v); // added
	V3 normalized(); // added
	float length(); // added
	V3 pointRotatedAboutAxis(V3 axisPoint1, V3 axisPoint2, float theta); // added
	V3 thisVectorRotatedAboutDirection(V3 direction, float theta); // added

	V3 UnitVector();
	float Length();
	V3 RotateThisPoint(V3 aO, V3 aD, float theta);
	V3 RotateThisVector(V3 aD, float theta);
	void SetFromColor(unsigned int col);
	unsigned int GetColor();
	V3 Light(V3 matColor, float ka, V3 ld, V3 norm);
	
	V3 LightWithSpecular(V3 matColor, float ka, V3 ld, V3 norm, float es, V3 eyePosition, V3 lightPosition, V3 thisPointPosition);



};
