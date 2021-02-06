#ifndef __DXF_ELLIPSE_H__
#define __DXF_ELLIPSE_H__
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

#ifndef __DXF_MATH_H__
    #include "Utils/DXFMath.h"
#endif

class CDXFEllipse : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFEllipse() : CDXFEntity("ELLIPSE") { }
    CDXFEllipse(const CDXF3DPoint &pointCenter, const CDXF3DPoint &pointEnd, double dMinorAxisRatio) : CDXFEntity("ELLIPSE"), 
        m_ptCenter(pointCenter), m_pointEnd(pointEnd), m_dMinorAxisRatio(dMinorAxisRatio) { }
    CDXFEllipse(const CDXFEllipse &ellipse) : CDXFEntity(ellipse, "ELLIPSE"),
        m_ptCenter(ellipse.m_ptCenter), m_pointEnd(ellipse.m_pointEnd), m_dMinorAxisRatio(ellipse.m_dMinorAxisRatio),
        m_dStartValue(ellipse.m_dStartValue), m_dEndValue(ellipse.m_dEndValue) { }
    virtual ~CDXFEllipse() { }

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFEllipse(*this); }

// Operations
public:
    const CDXF3DPoint &GetCenterPoint() const     { return m_ptCenter; }
    void SetCenterPoint(const CDXF3DPoint &point) { m_ptCenter = point; }

    const CDXF3DPoint &GetEndPoint() const        { return m_pointEnd; }

    const double GetStartValue() const { return m_dStartValue; }
    const double GetEndValue() const { return m_dEndValue; }
    const double GetMinorAxisRatio() const { return m_dMinorAxisRatio; }

// Atttributes
private:
    CDXF3DPoint m_ptCenter;
    CDXF3DPoint m_pointEnd;      // End point of major axis

    double m_dMinorAxisRatio {0.};
    double m_dStartValue     {0.};
    double m_dEndValue       {2.0 * DXF_PI};
};

#endif
