#ifndef __DXF_VECTOR_H__
#define __DXF_VECTOR_H__
#pragma once

#ifndef __DXF_3DPOINT_H__
	#include "DXF3DPoint.h"
#endif

class CDXFVector
{
// Construction/Destruction
public:
	CDXFVector() : x(0), y(0), z(0) { }
	CDXFVector(double X, double Y, double Z) : x(X), y(Y), z(Z) { }
	CDXFVector(const CDXF3DPoint &point) : x(point.x), y(point.y), z(point.z) { }
	~CDXFVector() { }

// Operators
public:
    operator CDXF3DPoint() { return CDXF3DPoint(x, y, z); } 
	CDXFVector operator*(double dScalar) {
    	return CDXFVector(x * dScalar, y * dScalar, z * dScalar);
    }
	double operator*(const CDXFVector &v) {
    	return (x * v.x + y * v.y + z * v.z);
    }

// Operations
public:
	void Normalize() {
	    const double dLength = Length();
	    x = x / dLength;
	    y = y / dLength;
	    z = z / dLength;
    }
    double Length() const { return ::sqrt(x * x + y * y + z * z); }

// Attributes
public:
	double x, y, z;
};

#endif
