#ifndef __DXF_BOUNDINGBOX_H__
#define __DXF_BOUNDINGBOX_H__
#pragma once

#ifndef __DXF_3DPOINT_H__
    #include "DXF3DPoint.h"
#endif

class CDXFBoundingBox final
{
// Construction/Detruction
public:
    CDXFBoundingBox() {
        m_minPoint.SetValues(1.E+10, 1.E+10, 1.E+10);
        m_maxPoint.SetValues(-1.E+10, -1.E+10, -1.E+10);
    }
    ~CDXFBoundingBox() { }

// Operations
public:
    void Union(const CDXFBoundingBox &bbox)
    {
        const CDXF3DPoint &minPoint = bbox.GetMinPoint();
        const CDXF3DPoint &maxPoint = bbox.GetMaxPoint();

        if (maxPoint.x > m_maxPoint.x) {
            m_maxPoint.x = maxPoint.x;
        }

        if (maxPoint.y > m_maxPoint.y) {
            m_maxPoint.y = maxPoint.y;
        }

        if (maxPoint.z > m_maxPoint.z) {
            m_maxPoint.z = maxPoint.z;
        }

        if (minPoint.x < m_minPoint.x) {
            m_minPoint.x = minPoint.x;
        }

        if (minPoint.y < m_minPoint.y) {
            m_minPoint.y = minPoint.y;
        }

        if (minPoint.z < m_minPoint.z) {
            m_minPoint.z = minPoint.z;
        }
    }

    void SetMinPoint(double x, double y, double z) { m_minPoint.SetValues(x, y, z); }
    void SetMinPoint(const CDXF3DPoint &point) { m_minPoint = point; }

    void SetMaxPoint(double x, double y, double z) { m_maxPoint.SetValues(x, y, z); }
    void SetMaxPoint(const CDXF3DPoint &point) { m_maxPoint = point; }

    const CDXF3DPoint &GetMinPoint() const { return m_minPoint; }
    const CDXF3DPoint &GetMaxPoint() const { return m_maxPoint; }

    double GetDeltaX() const { return (m_maxPoint.x - m_minPoint.x); }
    double GetDeltaY() const { return (m_maxPoint.y - m_minPoint.y); }
    double GetDeltaZ() const { return (m_maxPoint.z - m_minPoint.z); }

// Attributes
private:
    CDXF3DPoint m_minPoint;
    CDXF3DPoint m_maxPoint;
};

#endif
