#ifndef __DXF_ARC_H__
#define __DXF_ARC_H__
#pragma once

#ifndef __DXF_ENTITY_H__
	#include "DXFEntity.h"
#endif

#ifndef __DXF_3DPOINT_H__
	#include "../base/DXF3DPoint.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

#ifndef __DXF_LINE_WEIGHT_H__
    #include "../base/DXFLineWeight.h"
#endif

class CDXFArc : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFArc() : CDXFEntity("ARC") { }
	CDXFArc(const CDXFArc &arc)  : CDXFEntity(arc, "ARC"), 
          m_dEndAngle(arc.m_dEndAngle), 
          m_dRadius(arc.m_dRadius), 
          m_dStartAngle(arc.m_dStartAngle), 
          m_ptCenter(arc.m_ptCenter),
          m_eLineWeight(arc.m_eLineWeight){ }
	CDXFArc(CDXF3DPoint &center, double dRadius, double dStartAngle, double dEndAngle) : CDXFEntity("ARC"),
          m_ptCenter(center),  
          m_dRadius(dRadius), 
          m_dStartAngle(dStartAngle), 
          m_dEndAngle(dEndAngle) { }
    virtual ~CDXFArc() { }

// Overrides
public:
	virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFArc(*this); }

// Operations
public:
    void SetRadius(double dRadius) { m_dRadius = dRadius; }
    double GetRadius() const { return m_dRadius; }

    const CDXF3DPoint &GetCenterPoint() const     { return m_ptCenter; }
    void SetCenterPoint(const CDXF3DPoint &point) { m_ptCenter = point; }

    void SetLineWeight(EDXFLineWeight eLineWeight) { m_eLineWeight = eLineWeight; }
    EDXFLineWeight GetLineWeight() const { return m_eLineWeight; }

// Attributes
public:
    double m_dEndAngle   {0.};
    double m_dStartAngle {0.};
    double m_dRadius     {0.};
	CDXF3DPoint m_ptCenter;

private:
    EDXFLineWeight m_eLineWeight {EDXFLineWeight::LW_ByLayer}; // default: By Layer.
};

#endif
