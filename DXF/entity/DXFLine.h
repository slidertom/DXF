#ifndef __DXF_LINE_H__
#define __DXF_LINE_H__
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

class CDXFLine : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFLine() : CDXFEntity("LINE") { }
    CDXFLine(const CDXF3DPoint &start, const CDXF3DPoint &end) : CDXFEntity("LINE"), m_pointStart(start), m_pointEnd(end) { }
    CDXFLine(double x1, double y1, double x2, double y2) : CDXFEntity("LINE") { m_pointStart.SetValues(x1, y1, 0.); m_pointEnd.SetValues(x2, y2, 0.); }
    CDXFLine(const CDXFLine &line) : CDXFEntity(line, "LINE"), m_pointStart(line.m_pointStart), m_pointEnd(line.m_pointEnd), m_eLineWeight(line.m_eLineWeight) { }
    virtual ~CDXFLine() { }

// Overrides
public:
	virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFLine(*this); }

// Operations
public:
    const CDXF3DPoint &GetStartPoint() const { return m_pointStart; }
    void SetStartPoint(double x, double y, double z) { m_pointStart.SetValues(x, y, z); }

    const CDXF3DPoint &GetEndPoint() const { return m_pointEnd;   }
    void SetEndPoint(double x, double y, double z) { m_pointEnd.SetValues(x, y, z);   }

    void SetLineWeight(EDXFLineWeight eLineWeight) { m_eLineWeight = eLineWeight; }
    EDXFLineWeight GetLineWeight() const { return m_eLineWeight; }

// Attributes
public:
	CDXF3DPoint m_pointStart;
	CDXF3DPoint m_pointEnd;

private:
    EDXFLineWeight m_eLineWeight {EDXFLineWeight::LW_ByLayer}; // default: By Layer.
};

#endif
