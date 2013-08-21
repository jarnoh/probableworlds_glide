#ifndef _VECTOR3_H
#define _VECTOR3_H

/*
 * Vector3  - 3 dim vector
 * X0,Y0,Z0 - unit vectors
*/

#include <math.h>
#include <iostream.h>

class Vector3 {
public:
	float x,y,z;
public:
	Vector3() {x=0.0f;y=0.0f;z=0.0f;};
	Vector3(float x0, float y0, float z0) {x=x0; y=y0; z=z0;} ;
	Vector3(float *q) { x=q[0]; y=q[1]; z=q[2];}

	void operator =(Vector3 a);
	void operator +=(Vector3 a);
	void operator -=(Vector3 a);
	Vector3 operator +(Vector3 a);
	Vector3 operator -(Vector3 a);
	Vector3 operator *(float a);
	Vector3 operator /(float a);

	Vector3 operator -();

	Vector3 operator *(Vector3 a);	//cross product
	float dot(Vector3 a);					//dot product
	Vector3 in(Vector3 a);					//inner product

	float len();
	Vector3 normalize();

//  friend ostream &operator<<(ostream &strm, const Vector3 &a);
	friend class Matrix33;
	friend class Quat;
	friend class Camera;
};


/*
 * Unit vectors
 */

extern Vector3 X0;
extern Vector3 Y0;
extern Vector3 Z0;
/*
#define X0 Vector3(1,0,0)
#define Y0 Vector3(0,1,0)
#define Z0 Vector3(0,0,1)
*/

#endif
