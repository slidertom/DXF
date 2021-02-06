#ifndef __DXF_SOLID_H__
#define __DXF_SOLID_H__
#pragma once

#ifndef __DXF_ENTITY_H__
    #include "DXFEntity.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "base/DXF3DPoint.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

class CDXFSolid : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFSolid() : CDXFEntity("SOLID"), m_nPoints(3) { }
    CDXFSolid(const CDXFSolid &solid)
    : CDXFEntity(solid, "SOLID"),
      m_nPoints(solid.m_nPoints)
    {
        m_points[0] = m_points[0];
        m_points[1] = m_points[1];
        m_points[2] = m_points[2];
        m_points[3] = m_points[3];
    }
    CDXFSolid(const CDXF3DPoint &p1, const CDXF3DPoint &p2, const CDXF3DPoint &p3) : CDXFEntity("SOLID"), m_nPoints(3) {
        m_points[0] = p1;
        m_points[1] = p2;
        m_points[2] = p3;
    }
    CDXFSolid(const CDXF3DPoint &p1, const CDXF3DPoint &p2, const CDXF3DPoint &p3, const CDXF3DPoint &p4) : CDXFEntity("SOLID"), m_nPoints(4) {
        m_points[0] = p1;
        m_points[1] = p2;
        m_points[2] = p3;
        m_points[3] = p4;
    }
    virtual ~CDXFSolid() { }

// Operations
public:
    void SetPointAt(int32_t index, const CDXF3DPoint &point) {
        ASSERT(index >=0 && index <= 3);
        m_points[index] = point;
    }

    void SetNumPoints(int32_t nNumPoints) {
        if (nNumPoints < 3) {
            m_nPoints = 3;
        }
        else if (nNumPoints > 4) {
            m_nPoints = 4;
        }
        else {
            m_nPoints = nNumPoints;
        }
    }

    int32_t GetNumPoints() const { return m_nPoints; }

    bool GetPoint(int32_t nIndex, CDXF3DPoint &point) const {
        if (nIndex < m_nPoints) {
            point = m_points[nIndex];
            return true;
        }
        ASSERT(FALSE);
        return false;
    }

    CDXF3DPoint GetPoint(int32_t nIndex) const {
        if (nIndex < m_nPoints) {
            return m_points[nIndex];
        }
        ASSERT(FALSE);
        return CDXF3DPoint();
    }

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFSolid(*this); }

// Attributes
private:
    int32_t m_nPoints;
    CDXF3DPoint m_points[4];
};

#endif
