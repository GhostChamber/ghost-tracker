#ifndef MATRIX_H
#define MATRIX_H

class Matrix
{

public:

	//## **********************************************************************
	//## Constructor
	//## **********************************************************************
	Matrix();

	//## **********************************************************************
	//## Deconstructor
	//## **********************************************************************
	~Matrix();

	//## **********************************************************************
	//## LoadIdentity
	//##
	//## Sets the matrix equal to the identity matrix.
	//## **********************************************************************
	void LoadIdentity();

	//## **********************************************************************
	//## Translate
	//##
	//## Multiplies the matrix by a translation matrix.
	//##
	//## Input:
	//##   fX - x translation.
	//##   fY - y translation.
	//##   fZ - z translation.
	//## **********************************************************************
	void Translate(float fX,
		float fY,
		float fZ);

	//## **********************************************************************
	//## Rotate
	//##
	//## Multiplies the matrix by a rotation matrix.
	//##
	//## Input:
	//##   fAngle - angle of rotation in degrees.
	//##   fX - x component of axis of rotation.
	//##   fY - y component of axis of rotation.
	//##   fZ - z component of axis of rotation.
	//## **********************************************************************
	void Rotate(float fAngle,
		float fX,
		float fY,
		float fZ);

	//## **********************************************************************
	//## Scale
	//##
	//## Multiplies the matrix by a scale matrix.
	//##
	//## Input:
	//##   fX - x scale.
	//##   fY - y scale.
	//##   fZ - z scale.
	//## **********************************************************************
	void Scale(float fX,
		float fY,
		float fZ);

	//## **********************************************************************
	//## Frustum
	//##
	//## Multiplies the matrix by a frustum perspective matrix.
	//##
	//## Input:
	//##   fLeft   - left edge of near plane
	//##   fRight  - right edge of near plane
	//##   fBottom - bottom edge of near plane
	//##   fTop    - top edge of near plane
	//##   fNear   - distance of near plane from origin
	//##   fFar    - distance of far plane from origin
	//## **********************************************************************
	void Frustum(float fLeft,
		float fRight,
		float fBottom,
		float fTop,
		float fNear,
		float fFar);

	//## **********************************************************************
	//## Ortho
	//##
	//## Multiplies the matrix by a orthographic matrix.
	//##
	//## Input:
	//##   fLeft   - left edge of near plane
	//##   fRight  - right edge of near plane
	//##   fBottom - bottom edge of near plane
	//##   fTop    - top edge of near plane
	//##   fNear   - distance of near plane from origin
	//##   fFar    - distance of far plane from origin
	//## **********************************************************************
	void Ortho(float fLeft,
		float fRight,
		float fBottom,
		float fTop,
		float fNear,
		float fFar);

	//## **********************************************************************
	//## Inverse
	//## 
	//## Calculates the inverse of the current matrix and replaces the matrix
	//## data with the newly solved inverse. The Gauss-Jorden elimination
	//## method is used to determine the inverse. If no inverse exists, the 
	//## matrix is left unchanged.
	//## **********************************************************************
	void Inverse();

	//## **********************************************************************
	//## Transpose
	//## 
	//## Transposes the current matrix.
	//## **********************************************************************
	void Transpose();

	//## **********************************************************************
	//## Load
	//## 
	//## Loads a 4x4 matrix from a preallocated array of 16 floats.
	//##
	//## Input:
	//##   arValues - an array of float values to load into the Matrix
	//##              object. Float values should be arranged in column
	//##              major order.
	//## **********************************************************************
	void Load(float* arValues);

	//## **********************************************************************
	//## MultiplyVec3
	//##
	//## Multiplies this matrix by vector of size 3. The vector will be
	//## will be appended with a 4th component (w) value equal to 1.0f so that
	//## it can be properly multiplied with the 4x4 Matrix object.
	//## 
	//## Input:
	//##   arVec3 - a vector where arVec3[0] is the x component
	//##                           arVec3[1] is the y component
	//##                           arVec3[2] is the z component
	//##
	//## Output:
	//##   arRes - a vector of size 3 that contains the result of the multiply
	//##           operationion with the last component (w) removed.
	//## **********************************************************************
	void MultiplyVec3(const float* arVec3,
		float* arRes);

	void MultiplyVec3Dir(const float* arVec3,
		float* arRes);

	//## **********************************************************************
	//## MultiplyVec4
	//##
	//## Multiplies this matrix by vector of size 4.
	//## 
	//## Input:
	//##   arVec4 - a vector where arVec4[0] is the x component
	//##                           arVec4[1] is the y component
	//##                           arVec4[2] is the z component
	//##                           arVec4[3] is the w component.
	//##
	//## Output:
	//##   arRes - a vector of size 4 that contains the result of the multiply
	//##           operationion.
	//## **********************************************************************
	void MultiplyVec4(const float* arVec4,
		float* arRes);

	//## **********************************************************************
	//## Clear
	//##
	//## Sets all values to 0.
	//## **********************************************************************
	void Clear();

	//## **********************************************************************
	//## GetArray
	//##
	//## Returns a pointer to the Matrix's private float array of values.
	//## Matrix values are stored in column major format, which is expected
	//## when setting matrix uniforms in GLSL.
	//##
	//## Returns:
	//##   float* - pointer to matrix value array.
	//## **********************************************************************
	float* GetArray();

	//## **********************************************************************
	//## GetArrayValues
	//##
	//## Copies the matrix's value array to the pre-allocated size 16 float 
	//## array provided by the calling function.
	//##
	//## Output:
	//##   pArray - pointer to a preallocated size 16 float array.
	//## **********************************************************************
	void GetArrayValues(float* pArray);

	//## **********************************************************************
	//## operator*
	//##
	//## Allows two matrices to be multiplied.
	//##  matReturn = matThis * matOther
	//##
	//## Input:
	//##   matOther - matrix that will be multiplied against this matrix.
	//##
	//## Returns:
	//##   Matrix - a matrix containing result of multiplication.
	//## **********************************************************************
	Matrix operator*(const Matrix& matOther);

	//## **********************************************************************
	//## Multiply
	//##
	//## Multiplies the values of two matrices and stores them in a third pre-
	//## allocated size 16 float array.
	//##
	//## Input:
	//##   pOne - first array of values to multiply.
	//##   pTwo - second array of values to multiply.
	//##
	//## Output:
	//##   pResult - resulting matrix values of multiplication.
	//## **********************************************************************
	static void Multiply(float* pOne,
		float* pTwo,
		float* pResult);

	enum MatrixEnum
	{
		MATRIX_SIZE = 4,
		MATRIX_VALUE_COUNT = 16
	};

private:

	//## Array to hold the current matrix values in column major format.
	float m_arValues[MATRIX_VALUE_COUNT];

	//## An array to hold values for transformations (Rotate/Frustum/etc).
	static float s_arTransValues[MATRIX_VALUE_COUNT];

	//## An array to hold the result of multiplications
	static float s_arTempValues[MATRIX_VALUE_COUNT];
};

#endif
