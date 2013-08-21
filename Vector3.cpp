
#include "Vector3.h"

Vector3 X0(1,0,0);
Vector3 Y0(0,1,0);
Vector3 Z0(0,0,1);

/*
 * Basic operators
 */
void Vector3::operator =(Vector3 a) {
	x=a.x;
	y=a.y;
	z=a.z;
}
void Vector3::operator +=(Vector3 a) {
	x+=a.x;
	y+=a.y;
	z+=a.z;
}
void Vector3::operator -=(Vector3 a) {
	x-=a.x;
	y-=a.y;
	z-=a.z;
}

Vector3 Vector3::operator +(Vector3 a) {
	Vector3 r;
	r.x=x+a.x;
	r.y=y+a.y;
	r.z=z+a.z;
	return r;
}
Vector3 Vector3::operator -(Vector3 a) {
	Vector3 r;
	r.x=x-a.x;
	r.y=y-a.y;
	r.z=z-a.z;
	return r;
}
Vector3 Vector3::operator -() {
	Vector3 r;
	r.x=-x;
	r.y=-y;
	r.z=-z;
	return r;
}
Vector3 Vector3::operator *(float a) {
	Vector3 r;
	r.x = x*a;
	r.y = y*a;
	r.z = z*a;
	return r;
}
Vector3 Vector3::operator /(float a) {
	Vector3 r;
	r.x = x/a;
	r.y = y/a;
	r.z = z/a;
	return r;
}


Vector3 Vector3::operator *(Vector3 a) {
	Vector3 r;
	r.x = y*a.z - z*a.y;
	r.y = z*a.x - x*a.z;
	r.z = x*a.y - y*a.x;
	return r;
}

float Vector3::dot(Vector3 a) {
	return x*a.x + y*a.y + z*a.z;
}

Vector3 Vector3::in(Vector3 a) {
	Vector3 r;
	r.x = x*a.x;
	r.y = y*a.y;
	r.z = z*a.z;
	return r;
}


float Vector3::len() {
	float r;
	r=(float)sqrt(x*x+y*y+z*z);
	return r;
}

Vector3 Vector3::normalize() {
	Vector3 r;
	float l=len();
	if(l==0) l=0.00000000001f;
	r=(*this)/l;
	return r;
}

