#ifndef __DXF_3DPOINT_H__
#define __DXF_3DPOINT_H__
#pragma once

#ifndef __DXF_MATH_H__
    #include "../Utils/DXFMath.h"
#endif

class CDXF3DPoint  
{
public:
	union {
		struct {
			double x, y, z;
		};
		double coords[3];
	};

// Construction/Destruction
public:
	CDXF3DPoint() : x(0.0), y(0.0), z(0.0) { }
	CDXF3DPoint(double X, double Y, double Z) : x(X), y(Y), z(Z) { }
	~CDXF3DPoint() { }

// Operators
public:
    CDXF3DPoint &operator=(const CDXF3DPoint &point) {
        x = point.x;
	    y = point.y;
	    z = point.z;
	    return *this;
    }
	CDXF3DPoint &operator+=(const CDXF3DPoint &point) {
        x += point.x;
	    y += point.y;
	    z += point.z;
	    return *this;
    }
	CDXF3DPoint &operator-=(const CDXF3DPoint &point) {
	    x -= point.x;
	    y -= point.y;
	    z -= point.z;
	    return *this;
    }
	CDXF3DPoint &operator*=(double scalar) {
        x *= scalar;
	    y *= scalar;
	    z *= scalar;
	    return *this;
    }
	CDXF3DPoint &operator/=(double scalar) {
	    x /= scalar;
	    y /= scalar;
	    z /= scalar;
	    return *this;
    }

    CDXF3DPoint operator+(const CDXF3DPoint &point) { return CDXF3DPoint(x + point.x, y + point.y, z + point.z); }
    CDXF3DPoint operator-(const CDXF3DPoint &point) { return CDXF3DPoint(x - point.x, y - point.y, z - point.z); }

    CDXF3DPoint operator+(const CDXF3DPoint &point) const { return CDXF3DPoint(x + point.x, y + point.y, z + point.z); }
    CDXF3DPoint operator-(const CDXF3DPoint &point) const { return CDXF3DPoint(x - point.x, y - point.y, z - point.z); }
    
    CDXF3DPoint operator-(double offset) { return CDXF3DPoint(x - offset, y - offset, z - offset); }
    CDXF3DPoint operator+(double offset) { return CDXF3DPoint(x + offset, y + offset, z + offset); }

    CDXF3DPoint operator-(double offset) const { return CDXF3DPoint(x - offset, y - offset, z - offset); }
    CDXF3DPoint operator+(double offset) const { return CDXF3DPoint(x + offset, y + offset, z + offset); }

    const double &operator[](int32_t nIndex) const { return coords[nIndex]; }
    double &operator[](int32_t nIndex) { return coords[nIndex]; }

// Operations
public:
    void SetZeroes() { x = y = z = 0.0; }
    void SetAll(double num) { x = y = z = num; }
    void SetValues(double dx, double dy, double dz) { x = dx; y = dy; z = dz; }

    bool IsZero() const { return (dxf_math::eq0(x) && dxf_math::eq0(y) && dxf_math::eq0(z)); }

// Static operations
public:
    // Magnitude (length) of the vector (point coordinates).
	static double Magnitude(const CDXF3DPoint &point) {
		const double dMagnitude = ::sqrt(::pow(point.x, 2) + ::pow(point.y, 2) + ::pow(point.z, 2));
		return dMagnitude;
	}

    // Scalar product (dot product).
	static double DotP(const CDXF3DPoint &point1, const CDXF3DPoint &point2) {
		return point1.x * point2.x + point1.y * point2.y + point1.z * point2.z;
	}

    // The distance between this and the given coordinate.
    static double DistanceTo(CDXF3DPoint point1, CDXF3DPoint point2) {			
		point1 -= point2;
		return CDXF3DPoint::Magnitude(point1);
	}

    // Sets a new position for the point in polar coordinates.
	static void SetPolar(CDXF3DPoint &point, double dRadius, double dAngle) {
		point.x = dRadius * ::cos(dAngle);
		point.y = dRadius * ::sin(dAngle);
		point.z = 0.0;
	}

    // The angle from zero to this point (in rad).
	static double Angle(const CDXF3DPoint &point)
	{
		double dAngle = 0.0;
		const double dMagnitude = CDXF3DPoint::Magnitude(point);

		if (dxf_math::more(dMagnitude, 1.0e-6)) {
			const double dp = CDXF3DPoint::DotP(point, CDXF3DPoint(1.0, 0.0, 0.0));
			if (dp/dMagnitude >= 1.0) {
				dAngle = 0.0;
			}
			else if ( dxf_math::less(dp/dMagnitude, -1.0)) {
				dAngle = DXF_PI;
			}
			else {
        		dAngle = ::acos(dp / dMagnitude);
			}

			if ( dxf_math::less(point.y, 0.0)) {
				dAngle = 2*DXF_PI - dAngle;
			}
		}
		return dAngle;
	}

    // The angle from point1 and the point2 (in rad).
	static double AngleTo(const CDXF3DPoint &point1, const CDXF3DPoint &point2) {
        const CDXF3DPoint pt = point2 - point1;
		return CDXF3DPoint::Angle(pt);
	}

    static void MovePointOnLine(const CDXF3DPoint &pt1, const CDXF3DPoint &pt2, CDXF3DPoint &point, double offs) {
	    const double b = pt2.x - pt1.x;
	    const double a = pt2.y - pt1.y;
	    const double ratio = offs / sqrt(a*a+b*b);
	    point.x += b * ratio;
	    point.y += a * ratio;
    }
};

#endif 
