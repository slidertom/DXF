#ifndef __DXF_CIRCLE_H__
#define __DXF_CIRCLE_H__
#pragma once

#ifndef __DXF_ENTITY_H__
	#include "DXFEntity.h"
#endif

#ifndef __DXF_3DPOINT_H__
	#include "DXF3DPoint.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

#ifndef __DXF_LINE_WEIGHT_H__
    #include "base/DXFLineWeight.h"
#endif

class CDXFCircle : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFCircle() : CDXFEntity("CIRCLE") { } 
	CDXFCircle(const CDXF3DPoint &pt, double dRadius) : CDXFEntity("CIRCLE"), m_ptCenter(pt), m_dRadius(dRadius) { }
    CDXFCircle(const CDXFCircle &circle) : CDXFEntity(circle, "CIRCLE"), m_dRadius(circle.m_dRadius), m_ptCenter(circle.m_ptCenter), m_eLineWeight(circle.m_eLineWeight) { }
    virtual ~CDXFCircle() { }

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFCircle(*this); }

// Operations
public:
    void SetRadius(double dRadius) { m_dRadius = dRadius; }
    double GetRadius() const       { return m_dRadius; }

    const CDXF3DPoint &GetCenterPoint() const     { return m_ptCenter; }
    void SetCenterPoint(const CDXF3DPoint &point) { m_ptCenter = point; }

    void SetLineWeight(EDXFLineWeight eLineWeight) { m_eLineWeight = eLineWeight; }
    EDXFLineWeight GetLineWeight() const { return m_eLineWeight; }

// Attributes
private:
    double m_dRadius {0.};
	CDXF3DPoint m_ptCenter;
    EDXFLineWeight m_eLineWeight {EDXFLineWeight::LW_ByLayer}; // default: By Layer.
};

#endif
