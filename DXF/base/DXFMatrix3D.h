#ifndef __DXF_MATRIX3D_H__
#define __DXF_MATRIX3D_H__
#pragma once

#ifndef __DXF_3DPOINT_H__
    #include "DXF3DPoint.h"
#endif

class CDXFMatrix3D final
{
// Construction/Destruction
public:
    CDXFMatrix3D() { }
    ~CDXFMatrix3D() { }

// Operations
public:
    void SetIdentity() {
        SetZero();
        m11 = m22 = m33 = m44 = 1.0;
    }

    void SetZero() {
        ZeroMemory(m, sizeof m);
    }

    void SetRotationAboutZAxis(double angle);

// Operators
public:
    const double &operator()(int32_t nRow, int32_t nColumn) const { return m[nRow][nColumn]; }
          double &operator()(int32_t nRow, int32_t nColumn)       { return m[nRow][nColumn]; }

public:
    union {
        struct {
            double m11, m12, m13, m14;
            double m21, m22, m23, m24;
            double m31, m32, m33, m34;
            double m41, m42, m43, m44;
        };
        double m[4][4];
    };
};

inline void CDXFMatrix3D::SetRotationAboutZAxis(double angle)
{
    const double cosine = ::cos(angle);
    const double sinus  = ::sin(angle);

    SetIdentity();

    m11 = cosine;
    m12 = -sinus;

    m21 = sinus;
    m22 = cosine;
}

inline CDXFMatrix3D MatrixMultiply(const CDXFMatrix3D &m1, const CDXFMatrix3D &m2)
{
    int32_t i, j, k;
    CDXFMatrix3D tempMatrix;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            tempMatrix.m[i][j] = 0.0;
            for (k = 0; k < 4; ++k) {
                tempMatrix.m[i][j] += m1.m[i][k] * m2.m[k][j];
            }
        }
    }

    return tempMatrix;
}

inline CDXF3DPoint MatrixPointMultiply(const CDXFMatrix3D &m, const CDXF3DPoint &p)
{
    CDXF3DPoint resPoint;
    resPoint.x = m.m11 * p.x + m.m12 * p.y + m.m13 * p.z + m.m14;
    resPoint.y = m.m21 * p.x + m.m22 * p.y + m.m23 * p.z + m.m24;
    resPoint.z = m.m31 * p.x + m.m32 * p.y + m.m33 * p.z + m.m34;
    return resPoint;
}

#endif