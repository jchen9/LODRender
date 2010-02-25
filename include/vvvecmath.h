//////////////////////////////////////////////////////////////////////////////
// Filename:          vvvecmath.cpp
// Functionality:     Math library for vector graphics programming
//                    Use only for graphics programming. Usage for general math
//                    might fail because specific graphics optimizations are
//                    applied.
// Author:            Juergen Schulze-Doebold (schulze@hlrs.de)
// Institution:       University of Stuttgart, Supercomputing Center
// Comments:          Define VV_STANDALONE for demonstration
// Abbreviations:     deg = degrees (full circle = 360 degrees)
//                    rad = radiant (full circle = 2 * M_PI radiant)
//////////////////////////////////////////////////////////////////////////////

#ifndef _VVECMATH_H_
#define _VVECMATH_H_

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif

#include <iostream>
#include <iomanip>
#include <float.h>
#include <math.h>

//============================================================================
// Constant Definitions
//============================================================================
///< compiler independent definition for pi
const float VV_PI = 3.1415926535897932384626433832795028841971693993751058f; 
const float VV_FLT_MAX = FLT_MAX;   ///< maximum float value

//============================================================================
// Forward Declarations
//============================================================================

class vvVector3;
class vvVector4;
class vvMatrix3;
class vvMatrix4;

//============================================================================
// Class Definitions
//============================================================================

/** 3x3 matrix type. 
 Matrix elements are: e[row][column]
 @author Juergen Schulze-Doebold (schulze@hlrs.de)
*/
class vvMatrix3
{
  public:
    float e[3][3];   ///< matrix elements: [row][column]

    vvMatrix3();
    vvMatrix3(const vvMatrix3*);
    void print(const char*);
    void identity();
    void zero();
    void copy(const vvMatrix3*);
};

/** 4x4 matrix type. 
 Matrix elements are: e[row][column]
 @author Juergen Schulze-Doebold (schulze@hlrs.de)
*/
class vvMatrix4
{
  private:
    void LUDecomposition(int index[4], float &d);
    void LUBackSubstitution(int index[4], float b[4]);

  public:
    float e[4][4];   ///< matrix elements: [row][column]

    vvMatrix4();
    vvMatrix4(const vvMatrix4*);
    void print(const char*);
    void identity();
    void zero();
    void translate(float, float, float);
    void scale(float, float, float);
    void scale(float);
    void rotate(float, float, float, float);
    void multiplyPre(const vvMatrix4*);
    void multiplyPost(const vvMatrix4*);
    void transpose();
    float diagonal();
    void invertOrtho();
    void invert2D();
    void copy(const vvMatrix4*);
    void copyTrans(const vvMatrix4*);
    void copyRot(const vvMatrix4*);
    void transOnly();
    void rotOnly();
    void killTrans();
    void killRot();
    bool equal(const vvMatrix4*);    
    void makeGL(float*);
    void getGL(float*);
    void get(float*);
    void set(float*);
    void get(double*);
    void set(double*);
    void setRow(int, float, float, float, float);
    void setRow(int, vvVector3*);
    void setColumn(int, float, float, float, float);
    void setColumn(int, vvVector3*);
    void getRow(int, float*, float*, float*, float*);
    void getRow(int, vvVector3*);
    void getColumn(int, float*, float*, float*, float*);
    void getColumn(int, vvVector3*);
    void random(int, int);
    void random(float, float);
    void invert();
    void swapRows(int, int);
    void swapColumns(int, int);
    void setProjOrtho(float, float, float, float, float, float);
    void getProjOrtho(float*, float*, float*, float*, float*, float*);
    void setProjPersp(float, float, float, float, float, float);
    void getProjPersp(float*, float*, float*, float*, float*, float*);
    bool isProjOrtho();
    float getNearPlaneZ();
    void trackballRotation(int, int, int, int, int, int);
};

/** 3D vector primitive.
 @author Juergen Schulze-Doebold (schulze@hlrs.de)
*/
class vvVector4
{
  public:
    float e[4];       ///< vector elements (x|y|z|w)

    vvVector4();
    vvVector4(const vvVector4*);
    float &operator[](int);
    float operator[](int) const;
    void set(float, float, float, float);
    void multiply(const vvMatrix4*);
    void copy(const vvVector4*);
    void print(const char*);    
};

/** 3D vector primitive, also used for points
 @author Juergen Schulze-Doebold (schulze@hlrs.de)
*/
class vvVector3
{
  public:
    float e[3];       ///< vector elements (x|y|z)
    
    vvVector3();
    vvVector3(float, float, float);
    vvVector3(const vvVector3*);
    float &operator[](int);
    float operator[](int) const;
    void  set(float, float, float);
    void  get(float*, float*, float*);
    void  copy(const vvVector3*);
    void  copy(const vvVector4*);
    void  add(const vvVector3*);
    void  add(float);
    void  sub(const vvVector3*);
    void  sub(float);
    void  scale(float);
    void  scale(const vvVector3*);
    void  scale(float, float, float);
    float dot(const vvVector3*);
    float angle(vvVector3*);
    void  cross(const vvVector3*);
    void  multiply(const vvMatrix4*);
    void  multiply(const vvMatrix3*);
    float distance(const vvVector3*);
    float length();
    void  normalize();
    void  negate();
    bool  equal(const vvVector3*);    
    void  random(int, int);
    void  random(float, float);
    void  random(double, double);
    void  print(const char*);    
    void  getRow(const vvMatrix4*, int);
    void  getColumn(const vvMatrix4*, int);
    void  swap(vvVector3*);
    bool  isectPlaneLine(const vvVector3*, const vvVector3*, const vvVector3*, const vvVector3*);
    int   isectPlaneCuboid(const vvVector3*, const vvVector3*, const vvVector3*, const vvVector3*);
    int   isectRayCylinder(const vvVector3*, const vvVector3*, float, const vvVector3*, const vvVector3*);
    void  cyclicSort(int, const vvVector3*);
    void  zero();
    bool  isZero();
    void  getSpherical(float*, float*, float*);
    void  directionCosines(const vvVector3*);
};

#endif     

/////////////////
// End of File
/////////////////
