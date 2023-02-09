#pragma once

#include "V3.h"

using namespace std;


class M33 {
public:
	V3 rows[3];
	M33(); // added
	V3& operator[](int i);
	V3 operator*(V3 v);
	M33 operator*(M33 m); // added
	V3 GetColumn(int i);
	void SetColumn(int i, V3 c);
	void SetRotY(float theta);
	void SetRotX(float theta); // added
	void SetRotZ(float theta); // added
	void SetToIdentity(); // added
	friend ostream& operator<<(ostream& ostr, M33 m); // added
	friend istream& operator>>(istream& istr, M33 &m); // added

	M33 Inverted(); // already written for me
	M33 Transposed(); // added


};