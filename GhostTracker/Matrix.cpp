#pragma once

#define DEGREES_TO_RADIANS 0.0174532925f

#include "Matrix.h"
#include <math.h>
#include <string.h> // for memset, memcpy

float Matrix::s_arTransValues[MATRIX_VALUE_COUNT];
float Matrix::s_arTempValues[MATRIX_VALUE_COUNT];

//*****************************************************************************
// Constructor
//*****************************************************************************
Matrix::Matrix()
{
    LoadIdentity();
}

//*****************************************************************************
// Destructor
//*****************************************************************************
Matrix::~Matrix()
{

}

//*****************************************************************************
// LoadIdentity
//*****************************************************************************
void Matrix::LoadIdentity()
{
    int i = 0;
    int j = 0;

    for (i = 0; i < MATRIX_SIZE; i++)
    {
        for (j = 0; j < MATRIX_SIZE; j++)
        {
            m_arValues[i*MATRIX_SIZE + j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

//*****************************************************************************
// Translate
//*****************************************************************************
void Matrix::Translate(float fX,
                       float fY,
                       float fZ)
{
    // Clear the trans values array to hold new translation matrix.
    memset(s_arTransValues, 0, sizeof(float) * MATRIX_VALUE_COUNT);

    // Set to identity matrix
    s_arTransValues[0]  = 1.0f;
    s_arTransValues[5]  = 1.0f;
    s_arTransValues[10] = 1.0f;
    s_arTransValues[15] = 1.0f;

    // Add translation element
    s_arTransValues[12] = fX;
    s_arTransValues[13] = fY;
    s_arTransValues[14] = fZ;

    // Perform matrix multiplication with arrays
    Multiply(m_arValues, s_arTransValues, s_arTempValues);

    // Copy result back to value array
    memcpy(m_arValues, s_arTempValues, sizeof(float) * MATRIX_VALUE_COUNT);
}

//*****************************************************************************
// Rotate
//*****************************************************************************
void Matrix::Rotate(float fAngle,
                    float fX,
                    float fY,
                    float fZ)
{
    float fRadians = 0.0f;
    float fMagnitude = 0.0f;
    float c = 0.0f;
    float s = 0.0f;
    float x = fX;
    float y = fY;
    float z = fZ;

    // Convert angle in degrees to radians
    fRadians = fAngle * DEGREES_TO_RADIANS;

    // Calculate (once) the trig values needed in matrix generation.
    c = cos(fRadians);
    s = sin(fRadians);

    // Check if vector is normalized already.
    fMagnitude = sqrt(x*x + y*y + z*z);
    if (fMagnitude != 1.0f)
    {
        // Check for divide-by-zero errors
        if (fMagnitude == 0.0f)
        {
            // Don't do anything with improper arguments.
            return;
        }

        x = x/fMagnitude;
        y = y/fMagnitude;
        z = z/fMagnitude;
    }

    // Clear transformation values
    memset(s_arTransValues, 0, sizeof(float) * MATRIX_VALUE_COUNT);

    // Set to identity matrix
    s_arTransValues[0]  = 1.0f;
    s_arTransValues[5]  = 1.0f;
    s_arTransValues[10] = 1.0f;
    s_arTransValues[15] = 1.0f;

    // Add rotation element
    s_arTransValues[0] = x*x*(1 - c) + c;
    s_arTransValues[1] = y*x*(1 - c) + z*s;
    s_arTransValues[2] = x*z*(1 - c) - y*s;
    
    s_arTransValues[4] = x*y*(1 - c) - z*s;
    s_arTransValues[5] = y*y*(1 - c) + c;
    s_arTransValues[6] = y*z*(1 - c) + x*s;

    s_arTransValues[8]  = x*z*(1 - c) + y*s;
    s_arTransValues[9]  = y*z*(1 - c) - x*s;
    s_arTransValues[10] = z*z*(1 - c) + c;

    // Perform matrix multiplication with arrays
    Multiply(m_arValues, s_arTransValues, s_arTempValues);

    // Copy result back to value array
    memcpy(m_arValues, s_arTempValues, sizeof(float) * MATRIX_VALUE_COUNT);
}

//*****************************************************************************
// Scale
//*****************************************************************************
void Matrix::Scale(float fX,
                   float fY,
                   float fZ)
{
    // Clear the trans values array to hold new transformation matrix.
    memset(s_arTransValues, 0, sizeof(float) * MATRIX_VALUE_COUNT);

    // Set to identity matrix
    s_arTransValues[0]  = 1.0f;
    s_arTransValues[5]  = 1.0f;
    s_arTransValues[10] = 1.0f;
    s_arTransValues[15] = 1.0f;

    // Add scale element
    s_arTransValues[0]  = fX;
    s_arTransValues[5]  = fY;
    s_arTransValues[10] = fZ;

    // Perform matrix multiplication with arrays
    Multiply(m_arValues, s_arTransValues, s_arTempValues);

    // Copy result back to value array
    memcpy(m_arValues, s_arTempValues, sizeof(float) * MATRIX_VALUE_COUNT);
}

//*****************************************************************************
// Frustum
//*****************************************************************************
void Matrix::Frustum(float fLeft,
                     float fRight,
                     float fBottom,
                     float fTop,
                     float fNear,
                     float fFar)
{
    // Clear the trans values array to hold new transformation matrix.
    memset(s_arTransValues, 0, sizeof(float) * MATRIX_VALUE_COUNT);

    // Add frustum element (no need to set identity)
    s_arTransValues[0]  = (2*fNear)/(fRight - fLeft);
    s_arTransValues[5]  = (2*fNear)/(fTop - fBottom);
    s_arTransValues[8]  = (fRight + fLeft)/(fRight - fLeft);
    s_arTransValues[9]  = (fTop + fBottom)/(fTop - fBottom);
    s_arTransValues[10] = -1.0f * (fFar + fNear)/(fFar - fNear);
    s_arTransValues[11] = -1.0f;
    s_arTransValues[14] = -1.0f * (2*fFar*fNear)/(fFar - fNear);

    // Perform matrix multiplication with arrays
    Multiply(m_arValues, s_arTransValues, s_arTempValues);

    // Copy result back to value array
    memcpy(m_arValues, s_arTempValues, sizeof(float) * MATRIX_VALUE_COUNT);
}

//*****************************************************************************
// Ortho
//*****************************************************************************
void Matrix::Ortho(float fLeft,
                   float fRight,
                   float fBottom,
                   float fTop,
                   float fNear,
                   float fFar)
{
    // Clear the trans values array to hold new transformation matrix.
    memset(s_arTransValues, 0, sizeof(float) * MATRIX_VALUE_COUNT);

    // Set to identity matrix
    s_arTransValues[0]  = 1.0f;
    s_arTransValues[5]  = 1.0f;
    s_arTransValues[10] = 1.0f;
    s_arTransValues[15] = 1.0f;

    // Add orthographic element
    s_arTransValues[0]  =  2.0f/(fRight - fLeft);
    s_arTransValues[5]  =  2.0f/(fTop - fBottom);
    s_arTransValues[10] = -2.0f/(fFar - fNear);
    s_arTransValues[12] = -1.0f*(fRight + fLeft)/(fRight - fLeft);
    s_arTransValues[13] = -1.0f*(fTop + fBottom)/(fTop - fBottom);
    s_arTransValues[14] = -1.0f*(fFar + fNear)/(fFar - fNear);

    // Perform matrix multiplication with arrays
    Multiply(m_arValues, s_arTransValues, s_arTempValues);

    // Copy result back to value array
    memcpy(m_arValues, s_arTempValues, sizeof(float) * MATRIX_VALUE_COUNT);
}

//*****************************************************************************
// Inverse
//*****************************************************************************
void Matrix::Inverse()
{
    // This function uses Gauss-Jordan elimination for finding
    // the inverse of a 4x4 matrix.  If the inverse does not exist, than
    // the matrix is left alone and not altered in any way.
    static float arMat[MATRIX_VALUE_COUNT] = {0.0f};
    static float arInv[MATRIX_VALUE_COUNT] = {0.0f};
    static float arTmp[MATRIX_SIZE]        = {0.0f};

    int i = 0;
    int j = 0;
    int r = 0;
    int nInverseExists = 1;
    int nTarget        = 0;
    float fMax         = 0.0f;
    float fFactor      = 0.0f;

    // arMat will initially hold the current matrix values
    memcpy(arMat, m_arValues, MATRIX_VALUE_COUNT * sizeof(float));

    // arInv will initially hold the identity matrix, but by the end
    // of the algorithm, arInv will contain the inverse matrix.
    memset(arInv, 0, MATRIX_VALUE_COUNT * sizeof(float));
    arInv[0]  = 1.0f;
    arInv[5]  = 1.0f;
    arInv[10] = 1.0f;
    arInv[15] = 1.0f;

    // Iterate over columns
    for (j = 0; j < MATRIX_SIZE; j++)
    {
        nTarget = 0;
        fMax    = 0.0f;

        // First, find the row number that has the largest value in column j.
        // This row number will be called nTarget.
        for (i = j; i < MATRIX_SIZE; i++)
        {
            if (fabs(arMat[i + 4*j]) > fMax)
            {
                nTarget = i;
                fMax    = fabs(arMat[i + 4*j]);
            }
        }

        // If the maximum value was 0, then no inverse exists.
        if (fMax == 0.0f)
        {
            nInverseExists = 0;
            break;
        }

        // If the row with the max value is not equal to j, then swap
        // the topmost row with the target row. Now row j will have the
        // biggest leading number.
        if (nTarget != j)
        {
            // Make a backup of top row
            arTmp[0] = arMat[j + 0];
            arTmp[1] = arMat[j + 4];
            arTmp[2] = arMat[j + 8];
            arTmp[3] = arMat[j + 12];

            // Move target row to top
            arMat[j + 0]  = arMat[nTarget + 0];
            arMat[j + 4]  = arMat[nTarget + 4];
            arMat[j + 8]  = arMat[nTarget + 8];
            arMat[j + 12] = arMat[nTarget + 12];

            // Copy temp array values to the target row position
            arMat[nTarget + 0]  = arTmp[0];
            arMat[nTarget + 4]  = arTmp[1];
            arMat[nTarget + 8]  = arTmp[2];
            arMat[nTarget + 12] = arTmp[3];

            // Do the same exact operations on the arInv matrix.
            arTmp[0] = arInv[j + 0];
            arTmp[1] = arInv[j + 4];
            arTmp[2] = arInv[j + 8];
            arTmp[3] = arInv[j + 12];
            arInv[j + 0]  = arInv[nTarget + 0];
            arInv[j + 4]  = arInv[nTarget + 4];
            arInv[j + 8]  = arInv[nTarget + 8];
            arInv[j + 12] = arInv[nTarget + 12];
            arInv[nTarget + 0]  = arTmp[0];
            arInv[nTarget + 4]  = arTmp[1];
            arInv[nTarget + 8]  = arTmp[2];
            arInv[nTarget + 12] = arTmp[3];
        }

        // Now multiply row j by 1/arMat[j][j] to make leading number 1
        fFactor = 1/arMat[j + j*4];
        arMat[j + 0]  *= fFactor;
        arMat[j + 4]  *= fFactor;
        arMat[j + 8]  *= fFactor;
        arMat[j + 12] *= fFactor;
        arInv[j + 0]  *= fFactor;
        arInv[j + 4]  *= fFactor;
        arInv[j + 8]  *= fFactor;
        arInv[j + 12] *= fFactor;

        // Eliminate values in column j for all rows that arent row j
        for (r = 0; r < MATRIX_SIZE; r++)
        {
            if (r != j)
            {
                fFactor = -1.0f * arMat[r + j*4];
                arMat[r + 0]  += fFactor * arMat[j + 0];
                arMat[r + 4]  += fFactor * arMat[j + 4];
                arMat[r + 8]  += fFactor * arMat[j + 8];
                arMat[r + 12] += fFactor * arMat[j + 12];
                arInv[r + 0]  += fFactor * arInv[j + 0];
                arInv[r + 4]  += fFactor * arInv[j + 4];
                arInv[r + 8]  += fFactor * arInv[j + 8];
                arInv[r + 12] += fFactor * arInv[j + 12];
            }
        }
    }


    // Copy the arInv to the Matrix's m_arValues array.
    if (nInverseExists != 0)
    {
        memcpy(m_arValues, arInv, MATRIX_VALUE_COUNT * sizeof(float));
    }
    else
    {
        
    }
}

//*****************************************************************************
// Transpose
//*****************************************************************************
void Matrix::Transpose()
{
    int   i = 0;
    int   j = 0;
    float fTemp = 0.0f;

    for (i = 1; i < MATRIX_SIZE; i++)
    {
        for (j = 0; j < i; j++)
        {
            fTemp = m_arValues[i + j*4];
            m_arValues[i + j*4] = m_arValues[j + i*4];
            m_arValues[j + i*4] = fTemp;
        }
    }
}

//*****************************************************************************
// Load
//*****************************************************************************
void Matrix::Load(float* arValues)
{
    if (arValues != 0)
    {
        memcpy(m_arValues, arValues, MATRIX_VALUE_COUNT * sizeof(float));
    }
}

//*****************************************************************************
// MultiplyVec3
//*****************************************************************************
void Matrix::MultiplyVec3(const float* arVec3,
                          float* arRes)
{
    float arVec4[4] = {0.0f};
    float arRes4[4]  = {0.0f};

    if (arVec3 != 0)
    {
        // Add  an extra dimension to the vec3
        arVec4[0] = arVec3[0];
        arVec4[1] = arVec3[1];
        arVec4[2] = arVec3[2];
        arVec4[3] = 1.0f;

        MultiplyVec4(arVec4, arRes4);

        arRes[0] = arRes4[0];
        arRes[1] = arRes4[1];
        arRes[2] = arRes4[2];
    }
}

void Matrix::MultiplyVec3Dir(const float* arVec3,
                             float* arRes)
{
    float arVec4[4] = {0.0f};
    float arRes4[4]  = {0.0f};

    if (arVec3 != 0)
    {
        // Add  an extra dimension to the vec3
        arVec4[0] = arVec3[0];
        arVec4[1] = arVec3[1];
        arVec4[2] = arVec3[2];
        arVec4[3] = 0.0f;

        MultiplyVec4(arVec4, arRes4);

        arRes[0] = arRes4[0];
        arRes[1] = arRes4[1];
        arRes[2] = arRes4[2];
    }
}

//*****************************************************************************
// MultiplyVec4
//*****************************************************************************
void Matrix::MultiplyVec4(const float* arVec4,
                          float* arRes)
{
    int i = 0;
    int j = 0;

    // Clear the result vector
    arRes[0] = 0.0f;
    arRes[1] = 0.0f;
    arRes[2] = 0.0f;
    arRes[3] = 0.0f;

    if (arVec4 != 0)
    {
        for (i = 0; i < MATRIX_SIZE; i++)
        {
            for (j = 0; j < MATRIX_SIZE; j++)
            {
                arRes[i] += m_arValues[i + j*4] * arVec4[j];
            }
        }
    }
}

//*****************************************************************************
// Clear
//*****************************************************************************
void Matrix::Clear()
{
    memset(m_arValues, 0, sizeof(float) * MATRIX_VALUE_COUNT);
}

//*****************************************************************************
// GetArray
//*****************************************************************************
float* Matrix::GetArray()
{
    return m_arValues;
}

//*****************************************************************************
// GetArrayValues
//*****************************************************************************
void Matrix::GetArrayValues(float* pArray)
{
    if (pArray != 0)
    {
        memcpy(pArray, m_arValues, sizeof(float) * MATRIX_VALUE_COUNT);
    }
}

//*****************************************************************************
// operator*
//*****************************************************************************
Matrix Matrix::operator*(const Matrix& matOther)
{
    // Create result matrix to return
    Matrix matTwo = const_cast<Matrix&>(matOther);
    Matrix matResult;
    matResult.Clear();

    // Multiply the arrays of this and matOther
    Multiply(this->GetArray(),
             matTwo.GetArray(),
             matResult.GetArray());

    // Return the result matrix
    return matResult;
}

//*****************************************************************************
// Multiply
//*****************************************************************************
void Matrix::Multiply(float* pOne,
                      float* pTwo,
                      float* pResult)
{
    int i = 0;
    int j = 0;
    int k = 0;
    float fSum = 0.0f;

    for (i = 0; i < MATRIX_SIZE; i++)
    {
        for (j = 0; j < MATRIX_SIZE; j++)
        {
            fSum = 0.0f;
            for (k = 0; k < MATRIX_SIZE; k++)
            {
                fSum += pOne[j + k*4] * pTwo[i*4 + k];
            }
            pResult[i*4 + j] = fSum;
        }
    }
}
