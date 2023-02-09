
#include "M33.h"

M33::M33() {
	rows[0] = V3();
	rows[1] = V3();
	rows[2] = V3();
}

V3& M33::operator[](int i) {

	return rows[i];

}

V3 M33::operator*(V3 v) {

	M33 &m = *this;
	V3 ret;
	ret[0] = m[0] * v;
	ret[1] = m[1] * v;
	ret[2] = m[2] * v;
	return ret;

}


V3 M33::GetColumn(int i){

	return V3(rows[0][i], rows[1][i], rows[2][i]);

}

void M33::SetColumn(int i, V3 c) {

	rows[0][i] = c[0];
	rows[1][i] = c[1];
	rows[2][i] = c[2];

}

void M33::SetRotY(float theta) {

	float thetaRad = theta / 180.0f * 3.1415926f;
	float cost = cosf(thetaRad);
	float sint = sinf(thetaRad);
	M33 &m = *this;
	m[0] = V3(cost, 0.0f, sint);
	m[1] = V3(0.0f, 1.0f, 0.0f);
	m[2] = V3(-sint, 0.0f, cost);

}

void M33::SetRotX(float theta) {

	float thetaRad = theta / 180.0f * 3.1415926f;
	float cost = cosf(thetaRad);
	float sint = sinf(thetaRad);
	M33 &m = *this;
	m[0] = V3(1.0f, 0.0f, 0.0f);
	m[1] = V3(0.0f, cost, -sint);
	m[2] = V3(0.0f, sint, cost);

}

void M33::SetRotZ(float theta) {

	float thetaRad = theta / 180.0f * 3.1415926f;
	float cost = cosf(thetaRad);
	float sint = sinf(thetaRad);
	M33 &m = *this;
	m[0] = V3(cost, -sint, 0.0f);
	m[1] = V3(sint, cost, 0.0f);
	m[2] = V3(0.0f, 0.0f, 1.0f);

}


// #if 0
// ^ stands for cross product
// * for dot product
M33 M33::Inverted() {

	M33 ret;
	V3 a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
	V3 _a = b ^ c; _a = _a / (a * _a);
	V3 _b = c ^ a; _b = _b / (b * _b);
	V3 _c = a ^ b; _c = _c / (c * _c);
	ret[0] = _a;
	ret[1] = _b;
	ret[2] = _c;

	return ret;

}
// #endif

M33 M33::Transposed() {
	M33 ret;
	ret[0] = this->GetColumn(0);
	ret[1] = this->GetColumn(1);
	ret[2] = this->GetColumn(2);
	return ret;
}

void M33::SetToIdentity() {
	rows[0] = V3(1, 0, 0);
	rows[1] = V3(0, 1, 0);
	rows[2] = V3(0, 0, 1);
}

ostream& operator<<(ostream& ostr, M33 m) {
	return ostr << m[0] << " | " << m[1] << " | " << m[2];
}

istream& operator>>(istream& istr, M33 &m) {
	return istr >> m[0] >> m[1] >> m[2];
}

M33 M33::operator*(M33 m) {
	M33 &thisM = *this;
	M33 ret;

	ret.SetColumn(0, thisM * m.GetColumn(0));
	ret.SetColumn(1, thisM * m.GetColumn(1));
	ret.SetColumn(2, thisM * m.GetColumn(2));

	return ret;
}
