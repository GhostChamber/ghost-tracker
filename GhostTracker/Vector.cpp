#include "Vector.h"

Vector2::Vector2() :
	x(0.0f),
	y(0.0f)
{

}

Vector2::Vector2(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector3::Vector3() :
	x(0.0f),
	y(0.0f),
	z(0.0f)
{

}

Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector4::Vector4() :
	x(0.0f),
	y(0.0f),
	z(0.0f),
	w(0.0f)
{

}

Vector4::Vector4(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}
